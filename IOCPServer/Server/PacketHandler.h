#pragma once
#include <functional>
#include "SendBuffer.h"

using handleFunction = std::function<bool(PacketSessionRef&, BYTE*, uint16)>;
extern map<uint16, handleFunction> _packetHandler;

bool Handle_Echo_Test(PacketSessionRef& ref, Protocol::EchoTest pkt);

class PacketHandler
{
public:
	static void Init();
	static bool PakcetHandle(PacketSessionRef& session, BYTE* buffer, int32 len);

public:
	static SendBufferRef MakeSendBuffer(Protocol::EchoTest pkt);

	template<typename PacketData, typename PacketType>
	static SendBufferRef _MakeSendBuffer(PacketData& pkt, PacketType type);
	
};

template<typename PacketData, typename PacketType>
inline SendBufferRef PacketHandler::_MakeSendBuffer(PacketData& pkt, PacketType type)
{
	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	const uint16 packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBuf = make_shared<SendBuffer>(packetSize);
	PacketHeader* head = reinterpret_cast<PacketHeader*>(sendBuf->Buffer());
	head->type = type;
	head->size = packetSize;

	pkt.SerializeToArray(&head[1], dataSize);
	sendBuf->Close(packetSize);

	return sendBuf;
}

template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len)
{
	PacketData pkt;
	pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

	return func(ref, pkt);
}
