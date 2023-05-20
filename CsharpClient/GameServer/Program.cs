using GameServer.Packet;
using GameServer.ServerCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            NetworkManager.Instance.Connect();

            while (true)
            {
                NetworkManager.Instance.Update();
            }
        }
    }
}
