using Google.Protobuf;
using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Net.Mime.MediaTypeNames;

namespace GameServer.Packet
{
    public class PacketHandler
    {
        public static void HandleEchoTest(IMessage packet)
        {
            EchoTest pkt = packet as EchoTest;
            Console.WriteLine($"EchoData id : {pkt.Id}, string : { pkt.Txt}");

            NetworkManager.Instance.Send(packet, INGAME.Echo);
        }
    }
}
