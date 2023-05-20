#include "pch.h"
#include "Service.h"
#include "SendBuffer.h"
#include "ClinetSession.h"

#include <memory>
#include <utility>

class Test
{
public:
	void connect(int n) { }
};

template<typename T, typename Ret, typename... Args>
void Make(T* obj, std::function<void(T&, Args...)> memfunc, Args... args)
{
	memFunc(obj, std::forward<Args>(args)...);
}

int main()
{
	Test* test = new Test();

	//Make(test, &Test::connect, 10);
}
	//SocketUtils::Init();
	//PacketHandler::Init();

	//ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared<ClinetSession>));

	//service->Start();
	//for (int32 i = 0; i < 5; i++)
	//{
	//	GThreadManager->Launch([=]()
	//		{
	//			while (true)
	//			{
	//				service->GetIocpCore()->Dispatch();
	//			}
	//		});
	//}

	//GThreadManager->Join();

	//// 윈속 종료
	//SocketUtils::Clear();
