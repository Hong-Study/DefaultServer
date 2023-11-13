#pragma once
#include "Session.h"

class ClientSession : public PacketSession
{
public:
	ClientSession(boost::asio::io_context& context) : PacketSession(context) { }

	virtual void OnConnected() override;

	virtual void OnDisconnected() override;

	virtual void OnReadPacket(BYTE* buffer, uint64 len) override;

	virtual void OnWrite(uint64 len) override;
};