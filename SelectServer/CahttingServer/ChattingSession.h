#pragma once
#include <Session.h>

class ChattingSession : public PacketSession
{
public:
	ChattingSession();
	~ChattingSession();

	virtual void	OnConnected() override;
	virtual void	OnDisconnected() override;
	virtual int		OnSend(BYTE* buffer, int32 len) override;
	virtual int		OnRecvPacket(BYTE* buffer, int32 len) override;
};

