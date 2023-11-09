#include "pch.h"
#include "ClientSession.h"

void ClientSession::OnConnected()
{
	std::cout << "On Connected" << std::endl;
}

void ClientSession::OnDisconnected()
{

}

void ClientSession::OnReadPacket(BYTE* buffer, uint64 len)
{

}

void ClientSession::OnWrite(uint64 len)
{

}
