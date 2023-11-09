#pragma once
#include "RecvBuffer.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_context& context);
	virtual ~Session();

	void Start();
	void Write(SendBufferRef sendBuffer);
	void Disconnect();

	void Connect(std::string ip, int32 port);
	void Connect(tcp::endpoint endPoint);

	tcp::socket& GetSocket() { return _socket; }

protected:
	virtual void OnWrite(uint64 size) { }
	virtual uint64 OnRead(BYTE* buf, uint64 size) { return size; }
	virtual void OnConnected() { }
	virtual void OnDisconnected() { }

private:
	void AsyncWrite();
	void AsyncRead();
	void AsyncConnect(boost::asio::ip::tcp::endpoint& endpoint);
	void AsyncDisconnect(std::string cause);

	void ProcessWrite(const boost::system::error_code err, uint64 size);
	void ProcessRead(const boost::system::error_code& err, uint64 size);
	void ProcessConnect(const boost::system::error_code& err);

private:
	LockQueue<SendBufferRef>	_sendQueue;
	std::vector<SendBufferRef>	_sendVector;

	std::atomic<bool>				_isSend;
	RecvBuffer						_recvBuffer;
	tcp::socket						_socket;
};

struct PacketHeader
{
	uint16 size;
	uint16 type;
};

class PacketSession : public Session
{
public:
	PacketSession(boost::asio::io_context& context);
	virtual ~PacketSession();

protected:
	virtual uint64 OnRead(BYTE* buf, uint64 size) sealed;
	virtual void OnReadPacket(BYTE* buffer, uint64 size) abstract;
};