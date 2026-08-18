using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;

namespace StarCraftRecovery.Server;

internal enum LobbySlotKind
{
    Closed,
    Open,
    Computer,
    Human,
}

internal sealed class GameLobby
{
    public required int Identifier { get; init; }
    public required string Name { get; init; }
    public required string Host { get; init; }
    public required string Map { get; init; }
    public required int MaximumPlayers { get; init; }
    public required LobbySlotKind[] Slots { get; init; }
    public required int[] Races { get; init; }
    public Dictionary<ClientSession, int> Players { get; } = [];
    public Dictionary<int, string> Participants { get; } = [];
    public Dictionary<int, int> ResultReports { get; } = [];
    public SortedDictionary<uint, Dictionary<int, string>> PendingTurns { get; } = [];
    public bool Started { get; set; }
    public uint Seed { get; set; }
    public uint NextCommitTurn { get; set; }
}

internal sealed record GameResolution(int Identifier, int WinnerSlot,
                                      string WinnerName, string Reason,
                                      List<ClientSession> Targets);

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
        Console.WriteLine($"Battle.net relay service listening on {bindAddress}:{port}");
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
                                    "Starcraft Battle.Net Beta");
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
            case "SET_SLOT":
                await SetSlotAsync(session, fields).ConfigureAwait(false);
                break;
            case "LEAVE_GAME":
                await LeaveGameAsync(session).ConfigureAwait(false);
                break;
            case "START_GAME":
                await StartGameAsync(session).ConfigureAwait(false);
                break;
            case "TURN":
                await SubmitTurnAsync(session, fields).ConfigureAwait(false);
                break;
            case "GAME_RESULT":
                await SubmitGameResultAsync(session, fields).ConfigureAwait(false);
                break;
            case "GET_STATS":
                await SendStatsAsync(session).ConfigureAwait(false);
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
        await SendStatsAsync(session).ConfigureAwait(false);
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
            snapshot = games.Values.Where(game => !game.Started)
                .OrderBy(game => game.Identifier)
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
                Slots = Enumerable.Repeat(LobbySlotKind.Open, maximum).ToArray(),
                Races = Enumerable.Repeat(1, maximum).ToArray(),
            };
            game.Slots[0] = LobbySlotKind.Human;
            game.Players.Add(session, 0);
            games.Add(identifier, game);
            session.GameIdentifier = identifier;
        }
        await session.SendAsync("GAME_CREATED", game.Identifier.ToString(), game.Name,
                                game.Map, "0");
        await SendLobbyRosterAsync(game).ConfigureAwait(false);
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
            int slot = game is null ? -1 :
                Array.FindIndex(game.Slots, value => value == LobbySlotKind.Open);
            if (game is not null && !game.Started && slot >= 0 &&
                session.GameIdentifier is null)
            {
                game.Slots[slot] = LobbySlotKind.Human;
                game.Players.Add(session, slot);
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
        int playerSlot;
        lock (gate)
        {
            playerSlot = game.Players[session];
        }
        await session.SendAsync("JOINED_GAME", game.Identifier.ToString(), game.Name,
                                game.Map, playerSlot.ToString());
        await SendLobbyRosterAsync(game).ConfigureAwait(false);
    }

    private async Task SendLobbyRosterAsync(GameLobby game)
    {
        List<ClientSession> targets;
        List<(int Slot, LobbySlotKind Kind, int Race)> slots;
        List<(int Slot, string Name)> players;
        lock (gate)
        {
            targets = game.Players.Keys.ToList();
            slots = Enumerable.Range(0, game.MaximumPlayers)
                .Select(slot => (slot, game.Slots[slot], game.Races[slot]))
                .ToList();
            players = game.Players
                .Where(player => player.Key.AccountName is not null)
                .Select(player => (player.Value, player.Key.AccountName!))
                .OrderBy(player => player.Value)
                .ToList();
        }
        await BroadcastAsync(targets, "LOBBY_CLEAR");
        foreach ((int slot, LobbySlotKind kind, int race) in slots)
        {
            await BroadcastAsync(targets, "LOBBY_SLOT", slot.ToString(),
                                 kind.ToString().ToUpperInvariant(),
                                 race.ToString());
        }
        foreach ((int slot, string name) in players)
        {
            await BroadcastAsync(targets, "LOBBY_PLAYER", slot.ToString(), name);
        }
    }

    private async Task SetSlotAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 4 || !int.TryParse(fields[1], out int slot) ||
            !Enum.TryParse(fields[2], true, out LobbySlotKind kind) ||
            kind is LobbySlotKind.Human ||
            !int.TryParse(fields[3], out int race) || race is < 0 or > 2)
        {
            await session.SendAsync("ERROR", "SET_SLOT", "Invalid lobby slot.");
            return;
        }

        GameLobby? game = null;
        string? error = null;
        lock (gate)
        {
            if (session.GameIdentifier is not int identifier ||
                !games.TryGetValue(identifier, out game))
            {
                error = "Join a game first.";
            }
            else if (!string.Equals(game.Host, session.AccountName,
                                    StringComparison.OrdinalIgnoreCase))
            {
                error = "Only the host can change lobby slots.";
            }
            else if (game.Started || slot < 0 || slot >= game.MaximumPlayers ||
                     game.Slots[slot] == LobbySlotKind.Human)
            {
                error = "That lobby slot cannot be changed.";
            }
            else
            {
                game.Slots[slot] = kind;
                game.Races[slot] = race;
            }
        }
        if (error is not null || game is null)
        {
            await session.SendAsync("ERROR", "SET_SLOT",
                                    error ?? "The game is unavailable.");
            return;
        }
        await SendLobbyRosterAsync(game).ConfigureAwait(false);
    }

    private async Task StartGameAsync(ClientSession session)
    {
        GameLobby? game = null;
        List<ClientSession> targets = [];
        string? error = null;
        lock (gate)
        {
            if (session.GameIdentifier is not int identifier ||
                !games.TryGetValue(identifier, out game))
            {
                error = "Join a game first.";
            }
            else if (!string.Equals(game.Host, session.AccountName,
                                    StringComparison.OrdinalIgnoreCase))
            {
                error = "Only the host can start the game.";
            }
            else if (game.Started)
            {
                error = "The game has already started.";
            }
            else if (game.Players.Count < 2)
            {
                error = "At least two players are required.";
            }
            else
            {
                game.Started = true;
                game.Seed = (uint)RandomNumberGenerator.GetInt32(1, int.MaxValue);
                game.NextCommitTurn = 0;
                game.Participants.Clear();
                game.ResultReports.Clear();
                foreach ((ClientSession player, int slot) in game.Players)
                {
                    game.Participants[slot] = player.AccountName!;
                }
                targets = game.Players.Keys.ToList();
            }
        }
        if (error is not null || game is null)
        {
            await session.SendAsync("ERROR", "START_GAME",
                                    error ?? "The game is unavailable.");
            return;
        }
        await BroadcastAsync(targets, "GAME_START", game.Identifier.ToString(),
                             game.Map, game.Seed.ToString(),
                             game.Players.Count.ToString());
    }

    private async Task LeaveGameAsync(ClientSession session)
    {
        GameLobby? remainingGame = null;
        List<ClientSession> peers = [];
        GameResolution? resolution = null;
        bool closed = false;
        bool aborted = false;
        lock (gate)
        {
            if (session.GameIdentifier is int identifier &&
                games.TryGetValue(identifier, out GameLobby? game))
            {
                List<ClientSession> matchTargets = game.Players.Keys.ToList();
                int slot = -1;
                if (game.Players.TryGetValue(session, out int assignedSlot))
                {
                    slot = assignedSlot;
                    game.Players.Remove(session);
                }
                session.GameIdentifier = null;
                if (slot >= 0 && slot < game.Slots.Length)
                {
                    game.Slots[slot] = LobbySlotKind.Open;
                }
                peers = game.Players.Keys.ToList();
                if (game.Started)
                {
                    closed = true;
                    if (game.Participants.Count == 2 && game.Players.Count == 1)
                    {
                        int winnerSlot = game.Players.Values.Single();
                        resolution = FinishGameLocked(game, winnerSlot,
                                                      "FORFEIT", matchTargets);
                    }
                    else
                    {
                        AbortGameLocked(game, matchTargets);
                        aborted = true;
                    }
                }
                else
                {
                    closed = game.Players.Count == 0 ||
                        string.Equals(game.Host, session.AccountName,
                                      StringComparison.OrdinalIgnoreCase);
                    if (closed)
                    {
                        AbortGameLocked(game, matchTargets);
                    }
                    else
                    {
                        remainingGame = game;
                    }
                }
            }
            else
            {
                session.GameIdentifier = null;
            }
        }
        await session.SendAsync("LEFT_GAME");
        if (resolution is not null)
        {
            await SendResolutionAsync(resolution).ConfigureAwait(false);
        }
        else if (closed)
        {
            await BroadcastAsync(peers, "GAME_ABORTED",
                                 aborted ? "A player left the active game."
                                         : "The host closed the game.");
        }
        else if (remainingGame is not null)
        {
            await SendLobbyRosterAsync(remainingGame).ConfigureAwait(false);
        }
    }

    private async Task SubmitGameResultAsync(ClientSession session,
                                             string[] fields)
    {
        if (fields.Length < 2 || !int.TryParse(fields[1], out int winnerSlot))
        {
            await session.SendAsync("ERROR", "GAME_RESULT",
                                    "Invalid winning player slot.");
            return;
        }

        GameResolution? resolution = null;
        GameLobby? game = null;
        List<ClientSession> abortTargets = [];
        string? error = null;
        lock (gate)
        {
            if (session.GameIdentifier is not int identifier ||
                !games.TryGetValue(identifier, out game) ||
                !game.Started ||
                !game.Players.TryGetValue(session, out int reporterSlot))
            {
                error = "No active game.";
            }
            else if (!game.Participants.ContainsKey(winnerSlot))
            {
                error = "The winning slot is not an active player.";
            }
            else if (game.ResultReports.TryGetValue(reporterSlot,
                                                    out int previousWinner))
            {
                if (previousWinner != winnerSlot)
                {
                    error = "A different result was already submitted.";
                }
            }
            else
            {
                game.ResultReports.Add(reporterSlot, winnerSlot);
            }

            if (error is null && resolution is null &&
                game is not null &&
                game.ResultReports.Count == game.Participants.Count)
            {
                int[] winners = game.ResultReports.Values.Distinct().ToArray();
                if (winners.Length == 1)
                {
                    resolution = FinishGameLocked(game, winners[0], "RESULT");
                }
                else
                {
                    abortTargets = game.Players.Keys.ToList();
                    AbortGameLocked(game, abortTargets);
                    error = "Clients disagreed about the match result.";
                }
            }
        }
        if (error is not null)
        {
            if (abortTargets.Count != 0)
            {
                await BroadcastAsync(abortTargets, "GAME_ABORTED", error)
                    .ConfigureAwait(false);
            }
            else
            {
                await session.SendAsync("ERROR", "GAME_RESULT", error);
            }
            return;
        }
        if (resolution is not null)
        {
            await SendResolutionAsync(resolution).ConfigureAwait(false);
        }
        else
        {
            await session.SendAsync("GAME_RESULT_ACCEPTED",
                                    winnerSlot.ToString());
        }
    }

    private async Task SubmitTurnAsync(ClientSession session, string[] fields)
    {
        if (fields.Length < 3 || !uint.TryParse(fields[1], out uint turn) ||
            fields[2].Length > 4096 ||
            (fields[2] != "-" && (fields[2].Length % 2 != 0 ||
             fields[2].Any(character => !Uri.IsHexDigit(character)))))
        {
            await session.SendAsync("ERROR", "TURN", "Invalid turn payload.");
            return;
        }

        List<(List<ClientSession> Targets, string[] Fields)> commits = [];
        string? error = null;
        lock (gate)
        {
            if (session.GameIdentifier is not int identifier ||
                !games.TryGetValue(identifier, out GameLobby? game) ||
                !game.Started || !game.Players.TryGetValue(session, out int slot))
            {
                error = "No active game.";
            }
            else if (turn < game.NextCommitTurn || turn > game.NextCommitTurn + 64U)
            {
                error = "Turn is outside the receive window.";
            }
            else
            {
                if (!game.PendingTurns.TryGetValue(turn, out var submissions))
                {
                    submissions = [];
                    game.PendingTurns.Add(turn, submissions);
                }
                if (!submissions.TryAdd(slot, fields[2]))
                {
                    error = "Turn was already submitted.";
                }
                while (error is null &&
                       game.PendingTurns.TryGetValue(game.NextCommitTurn,
                                                     out var ready) &&
                       ready.Count == game.Players.Count)
                {
                    var commitFields = new List<string>
                    {
                        game.NextCommitTurn.ToString()
                    };
                    foreach ((int playerSlot, string payload) in ready.OrderBy(item => item.Key))
                    {
                        commitFields.Add(playerSlot.ToString());
                        commitFields.Add(payload);
                    }
                    commits.Add((game.Players.Keys.ToList(), commitFields.ToArray()));
                    game.PendingTurns.Remove(game.NextCommitTurn);
                    ++game.NextCommitTurn;
                }
            }
        }
        if (error is not null)
        {
            await session.SendAsync("ERROR", "TURN", error);
            return;
        }
        foreach (var commit in commits)
        {
            await BroadcastAsync(commit.Targets, "TURN_COMMIT", commit.Fields);
        }
    }

    private async Task RemoveSessionAsync(ClientSession session)
    {
        List<ClientSession> channelPeers = [];
        List<ClientSession> gamePeers = [];
        GameLobby? remainingGame = null;
        GameResolution? resolution = null;
        bool gameAborted = false;
        int departedSlot = -1;
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
                List<ClientSession> matchTargets = game.Players.Keys.ToList();
                if (game.Players.TryGetValue(session, out int assignedSlot))
                {
                    departedSlot = assignedSlot;
                    game.Players.Remove(session);
                }
                session.GameIdentifier = null;
                if (departedSlot >= 0 && departedSlot < game.Slots.Length)
                {
                    game.Slots[departedSlot] = LobbySlotKind.Open;
                }
                gamePeers = game.Players.Keys.ToList();
                if (game.Started)
                {
                    if (game.Participants.Count == 2 && game.Players.Count == 1)
                    {
                        int winnerSlot = game.Players.Values.Single();
                        resolution = FinishGameLocked(game, winnerSlot,
                                                      "FORFEIT", matchTargets);
                    }
                    else
                    {
                        AbortGameLocked(game, matchTargets);
                        gameAborted = true;
                    }
                }
                else
                {
                    gameAborted = game.Players.Count == 0 ||
                        string.Equals(game.Host, session.AccountName,
                                      StringComparison.OrdinalIgnoreCase);
                    if (gameAborted)
                    {
                        AbortGameLocked(game, matchTargets);
                    }
                    else
                    {
                        remainingGame = game;
                    }
                }
            }
        }
        if (session.AccountName is not null)
        {
            await BroadcastAsync(channelPeers, "USER_LEAVE", session.AccountName);
        }
        if (resolution is not null)
        {
            await SendResolutionAsync(resolution).ConfigureAwait(false);
        }
        else if (gameAborted)
        {
            await BroadcastAsync(gamePeers, "GAME_ABORTED",
                                 "A player left the game.");
        }
        else if (remainingGame is not null)
        {
            await SendLobbyRosterAsync(remainingGame).ConfigureAwait(false);
        }
    }

    private GameResolution FinishGameLocked(GameLobby game, int winnerSlot,
                                            string reason,
                                            List<ClientSession>? targets = null)
    {
        string winnerName = game.Participants[winnerSlot];
        string[] losers = game.Participants
            .Where(participant => participant.Key != winnerSlot)
            .Select(participant => participant.Value)
            .ToArray();
        if (!accounts.RecordGame(winnerName, losers))
        {
            throw new InvalidOperationException("The game result could not be recorded.");
        }
        List<ClientSession> recipients = targets ?? game.Players.Keys.ToList();
        games.Remove(game.Identifier);
        foreach (ClientSession player in recipients)
        {
            player.GameIdentifier = null;
        }
        Console.WriteLine($"Game {game.Identifier} finished: {winnerName} won ({reason}).");
        return new GameResolution(game.Identifier, winnerSlot, winnerName,
                                  reason, recipients);
    }

    private void AbortGameLocked(GameLobby game, List<ClientSession> targets)
    {
        games.Remove(game.Identifier);
        foreach (ClientSession player in targets)
        {
            player.GameIdentifier = null;
        }
    }

    private async Task SendResolutionAsync(GameResolution resolution)
    {
        await BroadcastAsync(resolution.Targets, "GAME_FINISHED",
                             resolution.Identifier.ToString(),
                             resolution.WinnerSlot.ToString(),
                             resolution.WinnerName, resolution.Reason)
            .ConfigureAwait(false);
        foreach (ClientSession target in resolution.Targets)
        {
            if (target.AccountName is not null)
            {
                try
                {
                    await SendStatsAsync(target).ConfigureAwait(false);
                }
                catch (Exception exception) when (exception is IOException or
                                                  SocketException or
                                                  ObjectDisposedException)
                {
                }
            }
        }
    }

    private async Task SendStatsAsync(ClientSession session)
    {
        if (session.AccountName is null)
        {
            return;
        }
        AccountStats stats = accounts.GetStats(session.AccountName);
        await session.SendAsync("PLAYER_STATS", session.AccountName,
                                stats.Wins.ToString(), stats.Losses.ToString());
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
            catch (Exception exception) when (exception is IOException or
                                              SocketException or
                                              ObjectDisposedException)
            {
            }
        }
    }
}
