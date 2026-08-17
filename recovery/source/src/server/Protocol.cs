using System.Globalization;
using System.Text;

namespace StarCraftRecovery.Server;

internal static class Protocol
{
    public const string Version = "RECOVERY-BNET/1";

    public static string Format(string command, params string[] fields)
    {
        var output = new StringBuilder(command);
        foreach (string field in fields)
        {
            output.Append('|');
            output.Append(Encode(field));
        }
        return output.ToString();
    }

    public static string[] Parse(string line)
    {
        string[] fields = line.Split('|');
        for (int index = 1; index < fields.Length; ++index)
        {
            fields[index] = Decode(fields[index]);
        }
        return fields;
    }

    public static string Encode(string value)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(value);
        var output = new StringBuilder(bytes.Length);
        foreach (byte valueByte in bytes)
        {
            bool safe = valueByte is >= (byte)'a' and <= (byte)'z' or
                                   >= (byte)'A' and <= (byte)'Z' or
                                   >= (byte)'0' and <= (byte)'9' or
                                   (byte)'-' or (byte)'_' or (byte)'.' or (byte)' ';
            if (safe && valueByte != (byte)'%')
            {
                output.Append((char)valueByte);
            }
            else
            {
                output.Append('%');
                output.Append(valueByte.ToString("X2", CultureInfo.InvariantCulture));
            }
        }
        return output.ToString();
    }

    public static string Decode(string value)
    {
        using var bytes = new MemoryStream(value.Length);
        for (int index = 0; index < value.Length; ++index)
        {
            if (value[index] == '%' && index + 2 < value.Length &&
                byte.TryParse(value.AsSpan(index + 1, 2),
                              NumberStyles.HexNumber,
                              CultureInfo.InvariantCulture, out byte decoded))
            {
                bytes.WriteByte(decoded);
                index += 2;
            }
            else
            {
                foreach (byte literal in Encoding.UTF8.GetBytes(value[index].ToString()))
                {
                    bytes.WriteByte(literal);
                }
            }
        }
        return Encoding.UTF8.GetString(bytes.ToArray());
    }
}
