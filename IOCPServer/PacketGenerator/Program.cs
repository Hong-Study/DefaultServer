
using System;
using System.IO;
using System.Reflection;
using System.Xml;

namespace PacketGenerator
{
    class Program
    {
        static void Main()
        {
            //MakeProto();
            MakeHandle();
        }
        static void MakeHandle()
        {
            string serverHandler = "";
            string serverInit = "";
            string serverMake = "";
            
            // 시작 기준은 bin/Debug/net6.0 안에 있는 exe 파일 기준
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
                serverHandler += string.Format(HandleFormat.handlerFormat, names[0]);

                // 초기화 코드 추가
                serverInit += string.Format(HandleFormat.initFormat, names[0]);

                // 생성 코드 (MakeSendBuffer) 추가
                serverMake += string.Format(HandleFormat.makeFormat, names[0]);
            }
            // 총 전체 코드로 합치기
            string serverManagerText = string.Format(HandleFormat.managerFormat, serverHandler, serverInit, serverMake);
            File.WriteAllText("PacketHandler.h", serverManagerText);

            // 만들어진 파일을 IOCP 서버로 옮기는 코드
            System.IO.File.Copy("PacketHandler.h", destPath, true);
        }

        static void MakeProto()
        {
            string handle = "";
            string file = "../../../../../Common/protoc-21.12-win64/bin/Enum.proto";
            string cppPath = "../../../../../Common/protoc-21.12-win64/bin/Protocol.proto";
            //string csharpPath = "../../../../Common/protoc-21.12-win64/bin/ProtocolC.proto";

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

                handle += String.Format(ProtoFormat.handleFormat, names[0]);
            }

            string cppText = string.Format(ProtoFormat.cppFormat, handle);
            string csharpText = string.Format(ProtoFormat.csharpFormat, handle);

            File.WriteAllText("Protocol.proto", cppText);
            //File.WriteAllText("ProtocolC.proto", csharpText);

            System.IO.File.Copy("Protocol.proto", cppPath, true);
            //System.IO.File.Copy("ProtocolC.proto", csharpPath, true);
        }
    }
}
