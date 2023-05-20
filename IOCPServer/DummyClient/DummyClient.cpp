#include "pch.h"
#include <iostream>
#include "Service.h"
#include "SendBuffer.h"

struct Tmp
{
	int32 id;
};

class Test : public PacketSession
{
public:
	virtual void OnConnected() override
	{
		cout << "OnConnect" << endl;
	}
	virtual void OnDisconnected() override
	{
		cout << "OnDisconnect" << endl;
	}
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		//PacketHeader* head = reinterpret_cast<PacketHeader*>(&buffer);

		//cout << "Input : " << head->id << " " << head->size << endl;

		//Send(buf);
	}
	virtual void OnSend(int32 len) override
	{
		cout << "Send Success " << len << endl;
	}
};

int main()
{
	SocketUtils::Init();
	::this_thread::sleep_for(2ms);

	cout << "시작" << endl;

	ClientServiceRef service = make_shared<ClientService>(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared<Test>));

	service->Start();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();

	// 윈속 종료
	SocketUtils::Clear();
}