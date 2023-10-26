using GameServer.Packet;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Collections.Generic;

namespace GameServer.ServerCore
{ 
    public class PacketHandler
    {
        #region Singleton
        static PacketHandler _instance = new PacketHandler();
        public static PacketHandler Instance { get { return _instance; } }
        #endregion

        Dictionary<UInt16, Action<ArraySegment<byte>, UInt16>> _onRecv = new Dictionary<UInt16, Action<ArraySegment<byte>, UInt16>>();
        Dictionary<UInt16, Action<IMessage>> _handle = new Dictionary<UInt16, Action<IMessage>>();

        public Action<UInt16, IMessage> CustomHandle {  get; set; }
        public PacketHandler() 
        {
            Init();
        }

        void Init()
        {
            _onRecv.Add((UInt16)INGAME.Inside, MakePacket<S_INSIDE>);
            _handle.Add((UInt16)INGAME.Inside, ClientPacketHandler.Handle_S_INSIDE);
            _onRecv.Add((UInt16)INGAME.Outside, MakePacket<S_OUTSIDE>);
            _handle.Add((UInt16)INGAME.Outside, ClientPacketHandler.Handle_S_OUTSIDE);
            _onRecv.Add((UInt16)INGAME.Start, MakePacket<S_START>);
            _handle.Add((UInt16)INGAME.Start, ClientPacketHandler.Handle_S_START);

        }

        public void ParsingPacket(ArraySegment<byte> buffer, PacketHeader head)
        {
            UInt16 id = head.type;

            Action<ArraySegment<byte>, UInt16> action = null;
            if(_onRecv.TryGetValue(id, out action))
            {
                action.Invoke(buffer, id);
            }
        }

        void MakePacket<T>(ArraySegment<byte> buffer, UInt16 id)where T: IMessage, new()
        {
            T pkt = new T();
            pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

            if(CustomHandle != null)
            {
                CustomHandle.Invoke(id, pkt);
            }
            else
            {
                Action<IMessage> action = null;
                if (_handle.TryGetValue(id, out action))
                {
                    action.Invoke(pkt);
                }
            }
        }

        public Action<IMessage> GetPakcetHandler(UInt16 id)
        {
            Action<IMessage> action = null;
            if (_handle.TryGetValue(id, out action))
            {
                return action;
            }

            return null;
        }
    }
}
