#pragma once
#include <functional>
#include "SendBuffer.h"

using handleFunction = std::function<bool(PacketSessionRef&, BYTE*, uint16)>;
extern map<uint16, handleFunction> _packetHandler;
template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len);

bool Handle_INSIDE(PacketSessionRef& ref, Protocol::C_INSIDE pkt);
bool Handle_OUTSIDE(PacketSessionRef& ref, Protocol::C_OUTSIDE pkt);
bool Handle_START(PacketSessionRef& ref, Protocol::C_START pkt);

class PacketHandler
{
public:
	static void Init()
	{
		_packetHandler[Protocol::INGAME::INSIDE] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_INSIDE>(Handle_INSIDE, ref, buf, size); };
		_packetHandler[Protocol::INGAME::OUTSIDE] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_OUTSIDE>(Handle_OUTSIDE, ref, buf, size); };
		_packetHandler[Protocol::INGAME::START] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_START>(Handle_START, ref, buf, size); };
		
	}
	static bool PakcetHandle(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);

		if (_packetHandler.find(head->type) == _packetHandler.end())
			return false;
		return _packetHandler[head->type](session, buffer, head->size);
	}

public:
	static SendBufferRef MakeSendBuffer(Protocol::S_INSIDE pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::INSIDE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_OUTSIDE pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::OUTSIDE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_START pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::START); }
	

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

