
using System;
using System.IO;
using System.Reflection;
using System.Xml;

namespace PacketGenerator
{
    class Program
    {
        static string serverHandler = "";
        static string serverInit = "";
        static string serverMake = "";
        

        static void Main()
        {
            string file = "../../../../../Common/protoc-21.12-win64/bin/Enum.proto";
            string destPath = "../../../../Server/PacketHandler.h";
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
                Console.WriteLine(names[0]);

                serverHandler += string.Format(PacketFormat.handlerFormat, names[0]);
                serverInit += string.Format(PacketFormat.initFormat, names[0]);
                serverMake += string.Format(PacketFormat.makeFormat, names[0]);
            }
            string serverManagerText = string.Format(PacketFormat.managerFormat, serverHandler, serverInit, serverMake);
            File.WriteAllText("PacketHandler.h", serverManagerText);
            System.IO.File.Copy("PacketHandler.h", destPath, true);
        }
    }
}
