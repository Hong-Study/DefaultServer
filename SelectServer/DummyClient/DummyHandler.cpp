#include "pch.h"
#include "DummyHandler.h"

void DummyHandler::Init()
{
	_handler[LOGIN] = [](BYTE* buf, int32 size, SessionRef ref) {Handler_Login(ParsingPacket(buf, size), ref)}();
}

void DummyHandler::Dispatch(BYTE* buffer, int32 len, SessionRef ref)
{
	PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);
	_handler[head->type](buffer, len, ref);
}

void DummyHandler::Handler_Login(Login&& login, SessionRef ref)
{

}

void DummyHandler::Handler_Chat(Chat&& chat, SessionRef ref)
{

}
