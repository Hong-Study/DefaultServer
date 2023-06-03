
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

                // 핸들러 코드 추가
                serverHandler += string.Format(PacketFormat.handlerFormat, names[0]);
                
                // 초기화 코드 추가
                serverInit += string.Format(PacketFormat.initFormat, names[0]);
                
                // 생성 코드 (MakeSendBuffer) 추가
                serverMake += string.Format(PacketFormat.makeFormat, names[0]);
            }
            // 총 전체 코드로 합치기
            string serverManagerText = string.Format(PacketFormat.managerFormat, serverHandler, serverInit, serverMake);
            File.WriteAllText("PacketHandler.h", serverManagerText);
            
            // 만들어진 파일을 IOCP 서버로 옮기는 코드
            System.IO.File.Copy("PacketHandler.h", destPath, true);
        }
    }
}
