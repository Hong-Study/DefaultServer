#include "pch.h"
#include "PacketHandler.h"
#include "ClinetSession.h"

map<uint16, handleFunction> _packetHandler;

bool Handle_ECHO(PacketSessionRef& ref, Protocol::C_ECHO pkt)
{
    cout << pkt.id() << " " << pkt.txt() << endl;

    Protocol::S_ECHO test;
    test.set_id(pkt.id());
    test.set_txt(pkt.txt());
    SendBufferRef sendBuf = PacketHandler::MakeSendBuffer(test);

    Gtest->PushJob(&TestManager::Send, ref, sendBuf);

    return true;
}
