namespace StarCraftRecovery.Server;

internal static class Program
{
    public static int Main(string[] args) =>
        MainAsync(args).GetAwaiter().GetResult();

    private static async Task<int> MainAsync(string[] args)
    {
        if (args.Contains("--help", StringComparer.OrdinalIgnoreCase) ||
            args.Contains("-h", StringComparer.OrdinalIgnoreCase))
        {
            Console.WriteLine("StarCraftRecoveryServer [--bind=ADDRESS] " +
                              "[--port=PORT] [--accounts=PATH]");
            Console.WriteLine("Defaults: --bind=127.0.0.1 --port=6112 and an " +
                              "account file beside the executable.");
            return 0;
        }
        if (args.Contains("--self-test", StringComparer.OrdinalIgnoreCase))
        {
            return RunSelfTest();
        }
        string bind = Argument(args, "--bind=") ?? "127.0.0.1";
        int port = int.TryParse(Argument(args, "--port="), out int parsedPort)
                       ? parsedPort : 6112;
        if (port is < 1 or > 65535)
        {
            Console.Error.WriteLine("Port must be between 1 and 65535.");
            return 2;
        }
        string accountPath = Argument(args, "--accounts=") ??
                             Path.Combine(AppContext.BaseDirectory,
                                          "StarCraftRecoveryServer.accounts.json");
        var accounts = new AccountStore(accountPath);
        var server = new BattleServer(bind, port, accounts);
        using var shutdown = new CancellationTokenSource();
        Console.CancelKeyPress += (_, eventArgs) =>
        {
            eventArgs.Cancel = true;
            shutdown.Cancel();
        };
        try
        {
            await server.RunAsync(shutdown.Token).ConfigureAwait(false);
            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception.Message);
            return 1;
        }
    }

    private static string? Argument(string[] args, string prefix) =>
        args.FirstOrDefault(value => value.StartsWith(prefix,
                                                       StringComparison.OrdinalIgnoreCase))
            ?[prefix.Length..];

    private static int RunSelfTest()
    {
        string sample = "A|B% test";
        if (Protocol.Decode(Protocol.Encode(sample)) != sample ||
            Protocol.Parse(Protocol.Format("CHAT", sample))[1] != sample)
        {
            return 10;
        }
        string directory = Path.Combine(Path.GetTempPath(),
                                        "StarCraftRecoveryServerSelfTest-" +
                                        Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(directory);
        try
        {
            string path = Path.Combine(directory, "accounts.json");
            var store = new AccountStore(path);
            if (!store.Create("Tester", "secret", out _) ||
                !store.Authenticate("tester", "secret") ||
                store.Authenticate("Tester", "wrong"))
            {
                return 11;
            }
            return 0;
        }
        finally
        {
            Directory.Delete(directory, true);
        }
    }
}
