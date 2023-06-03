using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PacketGenerator
{
    class PacketFormat
    {
		public static string managerFormat =
            @"#pragma once
#include <functional>
#include ""SendBuffer.h""

using handleFunction = std::function<bool(PacketSessionRef&, BYTE*, uint16)>;
extern map<uint16, handleFunction> _packetHandler;
template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len);

{0}
class PacketHandler
{{
public:
	static void Init()
	{{
		{1}
	}}
	static bool PakcetHandle(PacketSessionRef& session, BYTE* buffer, int32 len)
	{{
		PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);

		if (_packetHandler.find(head->type) == _packetHandler.end())
			return false;
		return _packetHandler[head->type](session, buffer, head->size);
	}}

public:
	{2}

	template<typename PacketData, typename PacketType>
	static SendBufferRef _MakeSendBuffer(PacketData& pkt, PacketType type);
	
}};

template<typename PacketData, typename PacketType>
inline SendBufferRef PacketHandler::_MakeSendBuffer(PacketData& pkt, PacketType type)
{{
	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	const uint16 packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBuf = make_shared<SendBuffer>(packetSize);
	PacketHeader* head = reinterpret_cast<PacketHeader*>(sendBuf->Buffer());
	head->type = type;
	head->size = packetSize;

	pkt.SerializeToArray(&head[1], dataSize);
	sendBuf->Close(packetSize);

	return sendBuf;
}}

template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len)
{{
	PacketData pkt;
	pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

	return func(ref, pkt);
}}

";
		public static string handlerFormat = @"bool Handle_{0}(PacketSessionRef& ref, Protocol::C_{0} pkt);
";
		public static string initFormat = @"_packetHandler[Protocol::INGAME::{0}] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) {{ return ParsingPacket<Protocol::C_{0}>(Handle_{0}, ref, buf, size); }};
";
		public static string makeFormat = @"static SendBufferRef MakeSendBuffer(Protocol::S_{0} pkt) {{ return _MakeSendBuffer(pkt, Protocol::INGAME::{0}); }}
";
    }
}
