using System;
using System.IO;
using System.Reflection;
using System.Xml;

class Program
{
    static string clientRegister;

    static void Main(string[] args)
    {
        string file = "../../../../../Common/protoc-21.12-win64/bin/Enum.proto";
        if (args.Length >= 1)
            file = args[0];

        bool startParsing = false;
        foreach (string line in File.ReadAllLines(file))
        {
            if (!startParsing && line.Contains("enum INGAME"))
            {
                startParsing = true;
                continue;
            }

            if (!startParsing)
                continue;

            if (line.Contains("{"))
                continue;

            if (line.Contains("NULL"))
                continue;

            if (line.Contains("}"))
                break;

            string[] names = line.Trim().Split(" =");
            if (names.Length == 0)
                continue;

            string name = names[0];
            string[] words = name.Split("_");

            string msgName = "";
            foreach (string word in words)
                msgName += FirstCharToUpper(word);

            string packetName = $"S_{name}";
            clientRegister += string.Format(PacketFormat.managerRegisterFormat, msgName, packetName);
        }

        string clientManagerText = string.Format(PacketFormat.managerFormat, clientRegister);
        File.WriteAllText("PacketManager.cs", clientManagerText);
    }

    public static string FirstCharToUpper(string input)
    {
        if (string.IsNullOrEmpty(input))
            return "";
        return input[0].ToString().ToUpper() + input.Substring(1).ToLower();
    }
}
