using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace StarCraftRecovery.Server;

internal sealed class GameLobby
{
    public required int Identifier { get; init; }
    public required string Name { get; init; }
    public required string Host { get; init; }
    public required string Map { get; init; }
    public required int MaximumPlayers { get; init; }
    public HashSet<ClientSession> Players { get; } = [];
}

internal sealed class ClientSession(TcpClient client)
{
    private readonly SemaphoreSlim sendGate = new(1, 1);
    public TcpClient Client { get; } = client;
    public StreamReader Reader { get; } =
        new(client.GetStream(), new UTF8Encoding(false), false, 4096, true);
    public StreamWriter Writer { get; } =
        new(client.GetStream(), new UTF8Encoding(false), 4096, true)
        { AutoFlush = true, NewLine = "\n" };
    public string? AccountName { get; set; }
    public string? Channel { get; set; }
    public int? GameIdentifier { get; set; }

    public async Task SendAsync(string command, params string[] fields)
    {
        await sendGate.WaitAsync().ConfigureAwait(false);
        try
        {
            await Writer.WriteLineAsync(Protocol.Format(command, fields))
                        .ConfigureAwait(false);
        }
        finally
        {
            sendGate.Release();
        }
    }
}

internal sealed class BattleServer(string bindAddress, int port,
                                   AccountStore accounts)
{
    private readonly object gate = new();
    private readonly TcpListener listener =
        new(IPAddress.Parse(bindAddress), port);
    private readonly ConcurrentDictionary<ClientSession, byte> sessions = [];
    private readonly Dictionary<string, HashSet<ClientSession>> channels =
        new(StringComparer.OrdinalIgnoreCase)
        {
            ["StarCraft"] = [],
            ["Brood War"] = [],
            ["Technical Support"] = [],
        };
    private readonly Dictionary<int, GameLobby> games = [];
    private int nextGameIdentifier;

    public async Task RunAsync(CancellationToken cancellationToken)
    {
        listener.Start();
        Console.WriteLine($"StarCraft recovery service listening on {bindAddress}:{port}");
        try
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                TcpClient client = await listener.AcceptTcpClientAsync(cancellationToken)
                                                 .ConfigureAwait(false);
                client.NoDelay = true;
                var session = new ClientSession(client);
                sessions.TryAdd(session, 0);
                _ = HandleClientAsync(session, cancellationToken);
            }
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
        }
        finally
        {
            listener.Stop();
            foreach (ClientSession session in sessions.Keys)
            {
                session.Client.Dispose();
            }
        }
    }

    private async Task HandleClientAsync(ClientSession session,
                                         CancellationToken cancellationToken)
    {
        string endpoint = session.Client.Client.RemoteEndPoint?.ToString() ?? "unknown";
        Console.WriteLine($"Client connected: {endpoint}");
        try
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                string? line = await session.Reader.ReadLineAsync(cancellationToken)
                                                   .ConfigureAwait(false);
                if (line is null)
                {
                    break;
                }
                if (line.Length > 8192)
                {
                    await session.SendAsync("ERROR", "LINE_TOO_LONG",
                                            "The request was too large.");
                    break;
                }
                await ProcessAsync(session, Protocol.Parse(line)).ConfigureAwait(false);
            }
        }
        catch (Exception exception) when (exception is IOException or SocketException or
                                          OperationCanceledException)
        {
        }
        finally
        {
            await RemoveSessionAsync(session).ConfigureAwait(false);
            session.Client.Dispose();
            sessions.TryRemove(session, out _);
            Console.WriteLine($"Client disconnected: {endpoint}");
        }
    }

    private async Task ProcessAsync(ClientSession session, string[] fields)
    {
        if (fields.Length == 0)
        {
            return;
        }
        string command = fields[0];
        if (command == "HELLO")
        {
            if (fields.Length < 2 || fields[1] != Protocol.Version)
            {
                await session.SendAsync("ERROR", "VERSION",
                                        "Unsupported recovery protocol version.");
                return;
            }
            await session.SendAsync("WELCOME", Protocol.Version,
                                    "StarCraft Recovery Service");
            return;
        }
        if (command == "CREATE_ACCOUNT")
        {
            if (fields.Length < 3)
            {
                await session.SendAsync("ERROR", "ARGUMENTS",
                                        "Account name and password are required.");
            }
            else if (accounts.Create(fields[1], fields[2], out string error))
            {
                await session.SendAsync("ACCOUNT_CREATED", fields[1]);
            }
            else
            {
                await session.SendAsync("ERROR", "ACCOUNT", error);
            }
            return;
        }
        if (command == "LOGON")
        {
            await LogonAsync(session, fields).ConfigureAwait(false);
            return;
        }
        if (session.AccountName is null)
        {
            await session.SendAsync("ERROR", "AUTH", "Log on first.");
            return;
        }
        switch (command)
        {
            case "LIST_CHANNELS":
                await ListChannelsAsync(session).ConfigureAwait(false);
                break;
            case "JOIN_CHANNEL":
                await JoinChannelAsync(session, fields).ConfigureAwait(false);
                break;
            case "CHAT":
                await ChatAsync(session, fields).ConfigureAwait(false);
                break;
            case "LIST_GAMES":
                await ListGamesAsync(session).ConfigureAwait(false);
                break;
            case "CREATE_GAME":
                await CreateGameAsync(session, fields).ConfigureAwait(false);
                break;
            case "JOIN_GAME":
                await JoinGameAsync(session, fields).ConfigureAwait(false);
                break;
            case "PING":
                await session.SendAsync("PONG");
                break;
            case "QUIT":
                session.Client.Close();
                break;
            default:
                await session.SendAsync("ERROR", "COMMAND", "Unknown request.");
                break;
        }
    }

    private async Task LogonAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 3 || !accounts.Authenticate(fields[1], fields[2]))
        {
            await session.SendAsync("ERROR", "LOGON",
                                    "The account name or password is incorrect.");
            return;
        }
        bool duplicate = sessions.Keys.Any(other => other != session &&
            string.Equals(other.AccountName, fields[1],
                          StringComparison.OrdinalIgnoreCase));
        if (duplicate)
        {
            await session.SendAsync("ERROR", "LOGON", "That account is already online.");
            return;
        }
        session.AccountName = fields[1];
        await session.SendAsync("LOGON_OK", session.AccountName);
    }

    private async Task ListChannelsAsync(ClientSession session)
    {
        List<(string Name, int Users)> snapshot;
        lock (gate)
        {
            snapshot = channels.Select(channel =>
                (channel.Key, channel.Value.Count)).ToList();
        }
        await session.SendAsync("CHANNEL_LIST_BEGIN");
        foreach ((string name, int users) in snapshot.OrderBy(item => item.Name))
        {
            await session.SendAsync("CHANNEL", name, users.ToString());
        }
        await session.SendAsync("CHANNEL_LIST_END");
    }

    private async Task JoinChannelAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 2 || string.IsNullOrWhiteSpace(fields[1]) ||
            fields[1].Length > 31)
        {
            await session.SendAsync("ERROR", "CHANNEL", "Invalid channel name.");
            return;
        }
        string channelName = fields[1].Trim();
        List<ClientSession> oldPeers = [];
        List<ClientSession> peers;
        lock (gate)
        {
            if (session.Channel is not null && channels.TryGetValue(session.Channel, out var old))
            {
                old.Remove(session);
                oldPeers = old.ToList();
            }
            if (!channels.TryGetValue(channelName, out var channel))
            {
                channel = [];
                channels[channelName] = channel;
            }
            channel.Add(session);
            session.Channel = channelName;
            peers = channel.ToList();
        }
        if (session.AccountName is not null)
        {
            await BroadcastAsync(oldPeers, "USER_LEAVE", session.AccountName);
        }
        await session.SendAsync("JOINED_CHANNEL", channelName);
        await session.SendAsync("USER_LIST_BEGIN");
        foreach (ClientSession peer in peers.Where(peer => peer.AccountName is not null)
                                             .OrderBy(peer => peer.AccountName))
        {
            await session.SendAsync("USER", peer.AccountName!);
        }
        await session.SendAsync("USER_LIST_END");
        if (session.AccountName is not null)
        {
            await BroadcastAsync(peers.Where(peer => peer != session),
                                 "USER_JOIN", session.AccountName);
            await BroadcastAsync(peers, "SYSTEM",
                                 $"{session.AccountName} joined the channel.");
        }
    }

    private async Task ChatAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 2 || string.IsNullOrWhiteSpace(fields[1]))
        {
            return;
        }
        if (fields[1].Length > 240 || session.Channel is null)
        {
            await session.SendAsync("ERROR", "CHAT", "Join a channel first.");
            return;
        }
        List<ClientSession> peers;
        lock (gate)
        {
            peers = channels.TryGetValue(session.Channel, out var channel)
                        ? channel.ToList() : [];
        }
        await BroadcastAsync(peers, "CHAT", session.AccountName!, fields[1]);
    }

    private async Task ListGamesAsync(ClientSession session)
    {
        List<(int Identifier, string Name, string Host, string Map,
              int Players, int MaximumPlayers)> snapshot;
        lock (gate)
        {
            snapshot = games.Values.OrderBy(game => game.Identifier)
                .Select(game => (game.Identifier, game.Name, game.Host,
                                 game.Map, game.Players.Count,
                                 game.MaximumPlayers))
                .ToList();
        }
        await session.SendAsync("GAME_LIST_BEGIN");
        foreach (var game in snapshot)
        {
            await session.SendAsync("GAME", game.Identifier.ToString(), game.Name,
                                    game.Host, game.Map, game.Players.ToString(),
                                    game.MaximumPlayers.ToString());
        }
        await session.SendAsync("GAME_LIST_END");
    }

    private async Task CreateGameAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 4 || string.IsNullOrWhiteSpace(fields[1]) ||
            fields[1].Length > 31 || !int.TryParse(fields[3], out int maximum) ||
            maximum is < 2 or > 12)
        {
            await session.SendAsync("ERROR", "GAME", "Invalid game advertisement.");
            return;
        }
        GameLobby game;
        lock (gate)
        {
            int identifier = ++nextGameIdentifier;
            game = new GameLobby
            {
                Identifier = identifier,
                Name = fields[1].Trim(),
                Host = session.AccountName!,
                Map = fields[2],
                MaximumPlayers = maximum,
            };
            game.Players.Add(session);
            games.Add(identifier, game);
            session.GameIdentifier = identifier;
        }
        await session.SendAsync("GAME_CREATED", game.Identifier.ToString(), game.Name);
    }

    private async Task JoinGameAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 2 || !int.TryParse(fields[1], out int identifier))
        {
            await session.SendAsync("ERROR", "GAME", "Invalid game identifier.");
            return;
        }
        GameLobby? game;
        lock (gate)
        {
            games.TryGetValue(identifier, out game);
            if (game is not null && game.Players.Count < game.MaximumPlayers)
            {
                game.Players.Add(session);
                session.GameIdentifier = identifier;
            }
            else
            {
                game = null;
            }
        }
        if (game is null)
        {
            await session.SendAsync("ERROR", "GAME", "That game is unavailable or full.");
            return;
        }
        await session.SendAsync("JOINED_GAME", game.Identifier.ToString(), game.Name,
                                game.Map);
    }

    private async Task RemoveSessionAsync(ClientSession session)
    {
        List<ClientSession> channelPeers = [];
        lock (gate)
        {
            if (session.Channel is not null && channels.TryGetValue(session.Channel, out var channel))
            {
                channel.Remove(session);
                channelPeers = channel.ToList();
            }
            if (session.GameIdentifier is int identifier &&
                games.TryGetValue(identifier, out GameLobby? game))
            {
                game.Players.Remove(session);
                if (game.Players.Count == 0 ||
                    string.Equals(game.Host, session.AccountName,
                                  StringComparison.OrdinalIgnoreCase))
                {
                    games.Remove(identifier);
                }
            }
        }
        if (session.AccountName is not null)
        {
            await BroadcastAsync(channelPeers, "USER_LEAVE", session.AccountName);
        }
    }

    private static async Task BroadcastAsync(IEnumerable<ClientSession> targets,
                                             string command,
                                             params string[] fields)
    {
        foreach (ClientSession target in targets)
        {
            try
            {
                await target.SendAsync(command, fields).ConfigureAwait(false);
            }
            catch (IOException)
            {
            }
        }
    }
}
