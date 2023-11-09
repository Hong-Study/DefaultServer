#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"

Session::Session(boost::asio::io_context& context)
	: _socket(context), _recvBuffer(1024), _isSend(false)
{

}

Session::~Session()
{
	std::cout << "Session 종료" << std::endl;
}

void Session::Start()
{
	OnConnected();

	AsyncRead();
}

void Session::Write(SendBufferRef sendBuffer)
{
	_sendQueue.Push(sendBuffer);

	bool expected = false;
	if (_isSend.compare_exchange_strong(expected, true))
	{
		AsyncWrite();
	}
}

void Session::Connect(std::string ip, int32 port)
{
	boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::make_address(ip), port);
	
	Connect(endPoint);
}

void Session::Connect(tcp::endpoint endPoint)
{
	AsyncConnect(endPoint);
}

void Session::Disconnect()
{
	if (_socket.is_open())
	{	
		_socket.close();
	}
}

void Session::AsyncWrite()
{
	if (uint64 size = _sendQueue.PopAll(_sendVector))
	{
		// 백터 고치기
		std::vector<BYTE> sendBuf(size);
		//uint64 pos = 0;
		//for (SendBufferRef ref : _sendVector)
		//{
		//	memcpy(sendBuf.data() + pos, ref->Buffer(), ref->WriteSize());
		//	pos += ref->WriteSize();
		//}

		// 세션 생명주기 넣기
		SessionRef sessionRef = shared_from_this();
		_socket.async_write_some(boost::asio::buffer(sendBuf),
			[sessionRef](const boost::system::error_code err, uint64 size)
			{
				sessionRef->ProcessWrite(err, size);
			});
	}
	else
		_isSend.store(false);
}

void Session::AsyncRead()
{
	// 세션 생명주기 넣기
	SessionRef ref = shared_from_this();
	_socket.async_read_some(boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()),
		[ref](const boost::system::error_code& err, uint64 size)
		{
			ref->ProcessRead(err, size);
		});
}

void Session::AsyncConnect(boost::asio::ip::tcp::endpoint& endpoint)
{
	SessionRef ref = shared_from_this();
	_socket.async_connect(endpoint, 
		[ref](const boost::system::error_code& err){
			ref->ProcessConnect(err);
		});
}

void Session::AsyncDisconnect(std::string cause)
{
	if (_socket.is_open())
	{
		OnDisconnected();

		std::cout << cause << std::endl;

		_socket.cancel();
		_socket.close();
	}
	else
		std::cout << cause << std::endl;
}

void Session::ProcessWrite(const boost::system::error_code err, uint64 size)
{
	if (!err) 
	{
		_sendVector.clear();

		OnWrite(size);

		if (_sendQueue.Size() == 0)
			_isSend.store(false);
		else
			AsyncWrite();
	}
	else
	{
		// TODO
		AsyncDisconnect(err.message());
	}
}

void Session::ProcessRead(const boost::system::error_code& err, uint64 size)
{
	if (!err)
	{
		if(_recvBuffer.OnWrite(size) == false)
		{
			AsyncDisconnect("Write Overflow");
			return;
		}
		int32 processLen = OnRead(_recvBuffer.ReadPos(), _recvBuffer.DataSize());
		if (processLen < 0 || size < processLen || _recvBuffer.OnRead(processLen) == false)
		{
			AsyncDisconnect("Read Overflow");
			return;
		}

		_recvBuffer.Clean();

		AsyncRead();
	}
	else
	{
		AsyncDisconnect(err.message());
	}
}

void Session::ProcessConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		Start();
	}
	else
	{
		AsyncDisconnect(err.message());
	}
}

PacketSession::PacketSession(boost::asio::io_context& context)
	: Session(context)
{

}

PacketSession::~PacketSession()
{
}

uint64 PacketSession::OnRead(BYTE* buffer, uint64 len)
{
	uint64 processLen = len;
	std::cout << len << std::endl;
	/*while (true)
	{
		uint64 dataSize = len - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader* head = reinterpret_cast<PacketHeader*>(&buffer[processLen]);
		if (head->size > dataSize)
			break;

		OnReadPacket(&buffer[processLen], head->size);

		processLen += head->size;
	}*/

	return processLen;
}
