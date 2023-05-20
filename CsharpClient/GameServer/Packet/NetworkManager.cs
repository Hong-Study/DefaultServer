using GameServer.ServerCore;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace GameServer.Packet
{
    class NetworkManager    
    {
        #region Singleton
        static NetworkManager _instance = new NetworkManager();
        public static NetworkManager Instance { get { return _instance; } }
        #endregion

        ClientSession session = new ClientSession();

        public NetworkManager() { }

        public void Connect()
        {
            PacketManager.Instance.CustomHandle = new Action<ushort, IMessage>(RecvPacketQueue.Instance.PushBack);
            session.Start(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7777));
        }
        public void Send(IMessage message, INGAME type)
        {
            session.Send(message, type);
        }

        public void Update()
        {
            List<Tuple<UInt16, IMessage>> values = RecvPacketQueue.Instance.PopAll();
            foreach(Tuple<UInt16, IMessage> value in values)
            {
                Action<IMessage> action = null;
                action = PacketManager.Instance.GetPakcetHandler(value.Item1);
                if (action != null)
                {
                    action.Invoke(value.Item2);
                }
            }
        }
    }
}
