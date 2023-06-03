#include "pch.h"
#include "ClinetSession.h"

void ClinetSession::OnConnected()
{
	cout << "OnConnect" << endl;
	Protocol::S_ECHO test;
	test.set_id(10);
	test.set_txt("Hello World");
	SendBufferRef ref = PacketHandler::MakeSendBuffer(test);

	//this_thread::sleep_for(1ms);
	Send(ref);
}

void ClinetSession::OnDisconnected()
{
	cout << "OnDisconnect" << endl;
}

void ClinetSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef ref = GetPacketSessionRef();

	if (PacketHandler::PakcetHandle(ref, buffer, len) == false)
		cout << "Recv Failed" << endl;

}

void ClinetSession::OnSend(int32 len)
{
	cout << "Send Success " << len << endl;
}
