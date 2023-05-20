#pragma once
#include <functional>

using Func = std::function<bool(BYTE*, uint32, SessionRef)>;
extern Func func[PKT_TYPE::COUNT];

bool HandlerChat(BYTE* buffer, uint32 len, SessionRef ref);
bool HandlerLogin(BYTE* buffer, uint32 len, SessionRef ref);

class PacketHandler
{
public:
	static void Init();
	static void Dispatch(BYTE* buffer, uint32 len, SessionRef ref);

public:
	static SendBufferRef MakeChat(string str);
	static SendBufferRef MakeLogin(S_Login&& datas);
};