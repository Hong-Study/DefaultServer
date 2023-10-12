#include "pch.h"
#include "ClinetSession.h"

void ClinetSession::OnConnected()
{

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
