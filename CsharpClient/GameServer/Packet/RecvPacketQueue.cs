using GameServer.ServerCore;
using Google.Protobuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameServer.Packet
{  
    public class RecvPacketQueue
    {
        #region Singleton
        static RecvPacketQueue _instance = new RecvPacketQueue();
        public static RecvPacketQueue Instance { get { return _instance; } }
        #endregion

        object _lock = new object();

        Queue<Tuple<UInt16, IMessage>> _queue = new Queue<Tuple<UInt16, IMessage>>();

        public void PushBack(UInt16 id, IMessage message) 
        {
            lock (_lock)
            {
                _queue.Enqueue(new Tuple<UInt16, IMessage>(id, message));
            }
        }

        public List<Tuple<UInt16, IMessage>> PopAll() 
        {
            List<Tuple<UInt16, IMessage>> _list = new List<Tuple<UInt16, IMessage>>();
            lock (_lock)
            {                
                if(_queue.Count != 0)
                    _list.Add(_queue.Dequeue());
            }
            return _list;
        }
    }
}
