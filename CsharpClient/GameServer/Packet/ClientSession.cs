using GameServer.ServerCore;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading.Tasks;

namespace GameServer.Packet
{
    public class ClientSession : PacketSession
    {
        public void Send(IMessage message, INGAME type)
        {
            int headSize = Marshal.SizeOf(typeof(PacketHeader));
            UInt16 pktSize = (UInt16)message.CalculateSize();
            PacketHeader header = new PacketHeader();
            header.size = pktSize;
            header.size += 4;
            header.type = (UInt16)type;
            byte[] sendBuffer = new byte[header.size];

            IntPtr ptr = Marshal.AllocHGlobal(headSize);
            Marshal.StructureToPtr(header, ptr, false);
            Marshal.Copy(ptr, sendBuffer, 0, headSize);
            Marshal.FreeHGlobal(ptr);

            Array.Copy(message.ToByteArray(), 0, sendBuffer, headSize, pktSize);
            Send(new ArraySegment<byte>(sendBuffer));
        }

        public override void OnConnected()
        {
            Console.WriteLine("On Connect");
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine("On Disconnect");
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer, PacketHeader header)
        {
            Console.WriteLine($"Header id : {header.type}, size : {header.size}");
            PacketHandler.Instance.ParsingPacket(buffer, header);
        }

        public override void OnSend(int size)
        {
           // TODO
        }
    }
}
