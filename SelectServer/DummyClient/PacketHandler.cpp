#include "pch.h"
#include "PacketHandler.h"

Func func[PKT_TYPE::COUNT];

SendBufferRef PacketHandler::MakeChat(string str)
{
	SendBufferRef ref = SENDBUF->Open(4096);

	BufferWriter writer(ref->Data(), 4096);
	PacketHeader* header = writer.Reserve<PacketHeader>();
	writer << str.data();

	header->size = writer.WriteSize();
	header->type = PKT_TYPE::CHAT;

	ref->Close(writer.WriteSize());
	return ref;
}

SendBufferRef PacketHandler::MakeLogin(S_Login&& data)
{
	SendBufferRef ref = SENDBUF->Open(4096);

	BufferWriter writer(ref->Data(), 4096);
	PacketHeader* header = writer.Reserve<PacketHeader>();
	writer << (S_Login)data;

	header->size = writer.WriteSize();
	header->type = PKT_TYPE::LOGIN;

	ref->Close(writer.WriteSize());
	return ref;
}

void PacketHandler::Init()
{
	for (int i = 0; i < PKT_TYPE::COUNT; i++)
	{
		func[i] = [](BYTE* buffer, uint32 len, SessionRef ref) {return false; };
	}
	func[PKT_TYPE::CHAT] = [](BYTE* buffer, uint32 len, SessionRef ref) {return HandlerChat(buffer, len, ref); };
	func[PKT_TYPE::LOGIN] = [](BYTE* buffer, uint32 len, SessionRef ref) {return HandlerLogin(buffer, len, ref); };
}

void PacketHandler::Dispatch(BYTE* buffer, uint32 len, SessionRef ref)
{
	PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);
	int32 pk = sizeof(PacketHeader);
	cout << head->type << " " << head->size << endl;
	if (!func[head->type](buffer + pk, len - pk, ref))
		cout << "비정상 실행 : " << head->type << endl;
}

bool HandlerChat(BYTE* buffer, uint32 len, SessionRef ref)
{
	BufferReader read(buffer, len);
	
	string str;
	read.Read((void*)str.data(), len);

	cout << str << endl;
	ref->Send(PacketHandler::MakeChat(str));
	return true;
}

bool HandlerLogin(BYTE* buffer, uint32 len, SessionRef ref)
{
	BufferReader read(buffer, len);

	S_Login ptr;
	read >> ptr;

	cout << ptr.id << " " << ptr.password << endl;

	ref->Send(PacketHandler::MakeLogin({ ptr.id, ptr.password }));

	return true;
}
