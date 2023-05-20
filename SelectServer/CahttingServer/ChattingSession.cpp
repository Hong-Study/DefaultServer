#include "pch.h"
#include "ChattingSession.h"
#include "PacketHandler.h"
#include <PacketType.h>

ChattingSession::ChattingSession()
{

}

ChattingSession::~ChattingSession()
{

}

void ChattingSession::OnConnected()
{

}

void ChattingSession::OnDisconnected()
{
}

int32 ChattingSession::OnSend(BYTE* buffer, int32 len)
{
	return len;
}

int32 ChattingSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketHandler::Dispatch(buffer, len, shared_from_this());
	return len;
}
