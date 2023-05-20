#pragma once

enum PKT_TYPE
{
	CHAT
	, LOGIN
	, COUNT
};

#pragma pack(1)
struct PacketHeader
{
	uint32 size;
	PKT_TYPE type;
};

struct S_Chat
{
	S_Chat(string chat) : _size(chat.length()), _chat(chat) { cout << _chat << endl; }
	uint8 _size;
	string _chat;
};

struct S_Login
{
	int32 id;
	int16 password;
};
#pragma pack()