#include "pch.h"
#include "PacketHandler.h"
#include "ClinetSession.h"

map<uint16, handleFunction> _packetHandler;

void PacketHandler::Init()
{
    _packetHandler[Protocol::INGAME::ECHO] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::EchoTest>(Handle_Echo_Test, ref, buf, size); };
}

bool PacketHandler::PakcetHandle(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);

    if (_packetHandler.find(head->type) == _packetHandler.end())
        return false;
    return _packetHandler[head->type](session, buffer, head->size);
}

SendBufferRef PacketHandler::MakeSendBuffer(Protocol::EchoTest pkt)
{
    return _MakeSendBuffer(pkt, Protocol::INGAME::ECHO);
}

bool Handle_Echo_Test(PacketSessionRef& ref, Protocol::EchoTest pkt)
{
    cout << pkt.id() << " " << pkt.txt() << endl;
    
    Protocol::EchoTest test;
    test.set_id(pkt.id());
    test.set_txt(pkt.txt());
    SendBufferRef sendBuf = PacketHandler::MakeSendBuffer(test);

    Gtest->PushJob(&TestManager::Send, ref, sendBuf);

    return true;
}
