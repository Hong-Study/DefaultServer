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

        public NetworkManager() 
        {
            
        }

        public void Connect()
        {
            session.Start(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7777));
        }
        public void Send(IMessage message, INGAME type)
        {
            session.Send(message, type);
        }
    }
}
