#include "pch.h"
#include "GameManager.h"
#include "service.h"

function<SessionRef(void)> factory = make_shared<DummySession>;

void GameManager::Init(int users)
{
	SocketUtils::Init();

	service = make_shared<ClientService>(Address(L"127.0.0.1", 5000), factory);

	service->Connect(users);

	service->Start();
}

void GameManager::Start()
{
	THREAD->Push([=]() {
		service->Start();
		});
}
