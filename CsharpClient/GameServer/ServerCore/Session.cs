using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GameServer.ServerCore
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PacketHeader
    {
        public UInt16 size;
        public UInt16 type;
    }
    public abstract class PacketSession : Session
    {
        public readonly int HeaderSize = Marshal.SizeOf(typeof(PacketHeader));
        
        public abstract void OnRecvPacket(ArraySegment<byte> buffer, PacketHeader header);
        public sealed override int OnRecv(ArraySegment<byte> buffer)
        {
            int processLen = 0;
            int packetCount = 0;

            while (true)
            {
                // 헤더 파싱 가능한지 확인
                if (buffer.Count < HeaderSize)
                    break;

                // 패킷이 완전체로 도착했는지 확인
                IntPtr ptr = Marshal.AllocHGlobal(HeaderSize);
                Marshal.Copy(buffer.Array, processLen, ptr, HeaderSize);
                PacketHeader head = (PacketHeader)Marshal.PtrToStructure(ptr, typeof(PacketHeader));
                Marshal.FreeHGlobal(ptr);

                if (buffer.Count < head.size)
                    break;

                Console.WriteLine(HeaderSize);
               
                // 여기까지 왔으면 패킷 조립 가능
                OnRecvPacket(new ArraySegment<byte>(buffer.Array, buffer.Offset, head.size), head);
                packetCount++;

                processLen += head.size;
                buffer = new ArraySegment<byte>(buffer.Array, buffer.Offset + head.size, buffer.Count - head.size);
            }

            if (packetCount > 1)
                Console.WriteLine($"패킷 모아보내기 : {packetCount}");

            return processLen;
        }
    }
    public abstract class Session
    {
        Socket _socket;
        Object _lock = new object();
        //bool _isSend = false; 

        RecvBuffer _recvBuffer = new RecvBuffer(4096);
        Queue<ArraySegment<byte>> _sendQueue = new Queue<ArraySegment<byte>>();
        List<ArraySegment<byte>> _sendPendding = new List<ArraySegment<byte>>();

        public abstract void OnConnected();
        public abstract int OnRecv(ArraySegment<byte> buffer);
        public abstract void OnSend(int size);
        public abstract void OnDisconnected(EndPoint endPoint);

        public void Start(IPEndPoint endPoint)
        {
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _socket.BeginConnect(endPoint, ConnectComplete, _socket);
        }

        public void Send(ArraySegment<byte> sendBuffer)
        {
            lock (_lock)
            {
                _sendQueue.Enqueue(sendBuffer);
                if (_sendPendding.Count == 0)
                {
                    RegisterSend();
                }
            }
        }

        public void Disconnect()
        {
            if (_socket.Connected)
            {
                _socket.BeginDisconnect(true, DisconnectComplete, _socket);
            }
        }

        void ConnectComplete(IAsyncResult result)
        {
            _socket.EndConnect(result);

            OnConnected();

            // 비동기 Recv 등록
            RegisterRecv(_recvBuffer.WriteSegment);
        }
        
        
        void DisconnectComplete(IAsyncResult result)
        {
            if (result.IsCompleted)
            {
                try
                {
                    _socket.EndDisconnect(result);

                    OnDisconnected(_socket.RemoteEndPoint);

                    _socket.Shutdown(SocketShutdown.Both);
                    _socket.Close();
                }
                catch (SocketException e)
                {
                    Console.WriteLine($"Disconnect Failed{e}");
                }
                catch (Exception e)
                {
                    Console.WriteLine($"Disconnect Failed{e}");
                }
            }
        }

        void RegisterRecv(ArraySegment<byte> bytes)
        {
            if (_socket.Connected)
            {
                // 등록해주고 더 해줄게 있을까?
                _socket.BeginReceive(bytes.Array, bytes.Offset, bytes.Count, SocketFlags.None, RecvEventComplete, _socket);
            }
        }
        
        void RecvEventComplete(IAsyncResult result)
        {
            if (result.IsCompleted)
            {
                try
                {
                    int size = _socket.EndReceive(result);
                    if (_recvBuffer.OnWrite(size) == false)
                        Console.WriteLine("RecvBuffer Write OverFlow");

                    // Protobuf 형태 변환
                    Console.WriteLine("Input Size " + size);

                    // 받은 정보 전달
                    OnRecv(_recvBuffer.ReadSegment);

                    if (_recvBuffer.OnRead(size) == false)
                        Console.WriteLine("RecvBuffer Read OverFlow");
                    // 정리
                    _recvBuffer.Clean();

                    // 재등록
                    RegisterRecv(_recvBuffer.WriteSegment);
                }
                catch (SocketException e)
                {
                    Console.WriteLine($"Recv Failed{e}");
                }
                catch (Exception e)
                {
                    Console.WriteLine($"Recv Failed{e}");
                }
            }
            else
            {
                // Error TODO
            }
        }

        void RegisterSend()
        {
            if (_socket.Connected)
            {
                while (_sendQueue.Count > 0)
                {
                    var value = _sendQueue.Dequeue();
                    _sendPendding.Add(value);
                }

                _socket.BeginSend(_sendPendding, SocketFlags.None, SendEventComplete, _socket);
            }
        }

        void SendEventComplete(IAsyncResult result)
        {
            lock (_lock)
            {
                if (result.IsCompleted)
                {
                    try
                    {
                        int size = _socket.EndSend(result);
                        if (size > 0)
                        {
                            _sendPendding.Clear();

                            OnSend(size);

                            if (_sendQueue.Count > 0)
                                RegisterSend();
                        }

                    }
                    catch (SocketException e)
                    {
                        Console.WriteLine($"Send Failed{e}");
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine($"Send Failed{e}");
                    }
                }
                else
                {
                    //TODO
                }
            }
        }
    }
}
