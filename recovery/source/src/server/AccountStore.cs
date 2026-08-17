using System.Security.Cryptography;
using System.Text.Json;

namespace StarCraftRecovery.Server;

internal sealed class AccountRecord
{
    public required string Name { get; init; }
    public required string Salt { get; init; }
    public required string PasswordHash { get; init; }
}

internal sealed class AccountStore
{
    private const int Iterations = 120_000;
    private readonly object gate = new();
    private readonly string path;
    private readonly Dictionary<string, AccountRecord> accounts =
        new(StringComparer.OrdinalIgnoreCase);

    public AccountStore(string path)
    {
        this.path = path;
        Load();
    }

    public bool Create(string name, string password, out string error)
    {
        if (!ValidName(name))
        {
            error = "Account names must be 2-15 letters, numbers, or _-[] characters.";
            return false;
        }
        if (password.Length is < 4 or > 64)
        {
            error = "Passwords must contain 4-64 characters.";
            return false;
        }
        lock (gate)
        {
            if (accounts.ContainsKey(name))
            {
                error = "That account already exists.";
                return false;
            }
            byte[] salt = RandomNumberGenerator.GetBytes(16);
            byte[] hash = Hash(password, salt);
            accounts.Add(name, new AccountRecord
            {
                Name = name,
                Salt = Convert.ToBase64String(salt),
                PasswordHash = Convert.ToBase64String(hash),
            });
            Save();
        }
        error = string.Empty;
        return true;
    }

    public bool Authenticate(string name, string password)
    {
        AccountRecord? account;
        lock (gate)
        {
            accounts.TryGetValue(name, out account);
        }
        if (account is null)
        {
            return false;
        }
        byte[] expected = Convert.FromBase64String(account.PasswordHash);
        byte[] actual = Hash(password, Convert.FromBase64String(account.Salt));
        return CryptographicOperations.FixedTimeEquals(actual, expected);
    }

    private static bool ValidName(string name) =>
        name.Length is >= 2 and <= 15 &&
        name.All(character => char.IsAsciiLetterOrDigit(character) ||
                              character is '_' or '-' or '[' or ']');

    private static byte[] Hash(string password, byte[] salt) =>
        Rfc2898DeriveBytes.Pbkdf2(password, salt, Iterations,
                                 HashAlgorithmName.SHA256, 32);

    private void Load()
    {
        if (!File.Exists(path))
        {
            return;
        }
        AccountRecord[] records =
            JsonSerializer.Deserialize<AccountRecord[]>(File.ReadAllText(path)) ?? [];
        foreach (AccountRecord record in records)
        {
            accounts[record.Name] = record;
        }
    }

    private void Save()
    {
        string? directory = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(directory))
        {
            Directory.CreateDirectory(directory);
        }
        string temporary = path + ".tmp";
        var options = new JsonSerializerOptions { WriteIndented = true };
        File.WriteAllText(temporary,
                          JsonSerializer.Serialize(accounts.Values
                              .OrderBy(account => account.Name)
                              .ToArray(), options));
        File.Move(temporary, path, true);
    }
}
