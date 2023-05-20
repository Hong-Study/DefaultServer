#include "pch.h"
#include "DummySession.h"
#include "PacketHandler.h"

DummySession::DummySession()
{
	
}

DummySession::~DummySession()
{

}

void DummySession::OnConnected()
{	
	cout << "OnConnected" << endl;
	/*SendBufferRef ref = PacketHandler::MakeChat({ "Hello World" });

	Send(ref);*/
}

void DummySession::OnDisconnected()
{
	cout << "OnDisconnected" << endl;
}

int32 DummySession::OnSend(BYTE* buffer, int32 len)
{
	return len;
}

int32 DummySession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketHandler::Dispatch(buffer, len, shared_from_this());

	return len;
}