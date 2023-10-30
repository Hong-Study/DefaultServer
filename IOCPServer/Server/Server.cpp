#include "pch.h"
#include "Service.h"
#include "SendBuffer.h"
#include "ClinetSession.h"

#include <memory>
#include <utility>

void DoWork(ServerServiceRef& service)
{
	while (true)
	{
		service->GetIocpCore()->Dispatch(10);

		GThreadManager->DoGlobalQueue();

		GThreadManager->DoGlobalTimer();
	}
}

int main()
{
	SocketUtils::Init();
	PacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared<ClinetSession>));

	service->Start();
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWork(service);
				}
			});
	}

	while (1)
	{
		Gtest->DoTimer(1000, &TestManager::Send, string("Hello World"));
	}

	GThreadManager->Join();

	// 윈속 종료
	SocketUtils::Clear();
}