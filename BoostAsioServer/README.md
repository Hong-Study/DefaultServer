# C++ IOCP Server

![image](https://github.com/Hong-Study/DefaultServer/assets/48054868/2309c8ef-d3ec-4bf3-aa13-c73a83172887)

### 설명
- 기본적으로 클라이언트가 접속하면 Session 형태로 관리됩니다. ![ClientSession](https://github.com/Hong-Study/DefaultServer/blob/main/IOCPServer/Server/ClinetSession.h)
- 패킷의 경우, Protobuf를 사용하여 Send, Recv를 하게 되며 해당 핸들러 부분은 ![ServerPacketHandler](https://github.com/Hong-Study/DefaultServer/blob/main/IOCPServer/Server/PacketHandler.h) 에 담겨있습니다.


### 실행 방식
1. ServerService 를 통해, Session 형태와 NetAddress 를 집어넣음 
```ServerServiceRef service = make_shared(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared));```
2. service->Start() 실행
3. 스레드를 만들어 service->GetIocpCore()->Dispatch() 실행, 단 시간 초를 정하고 싶으면 Dispatch의 인자로 넘겨주면 됨.
4. GThreadManager->Join(); 을 통해 대기
```
enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		//LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		GThreadManager->DoGlobalQueue();

		// 예약된 일감 처리
		GThreadManager->DoGlobalTimer();
	}
}
int main()
{
	// CALL $(SolutionDir)PacketGenerator\bin\Debug\net6.0\PacketGenerator.exe

	ServerPacketHandler::Init();
	SocketUtils::Init();

	ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 4000), 10, std::function<SessionRef()>(make_shared<GameSession>));

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});		
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}
```

5. Recv를 받으면 PacketHandler를 통해 타입에 맞는 함수 실행
단, Protocol.proto 에 구현된 패킷 형태의 함수를 맞춰서 집어넣어 줘야함.
```
void ClinetSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef ref = GetPacketSessionRef();

	if (PacketHandler::PakcetHandle(ref, buffer, len) == false)
		cout << "Recv Failed" << endl;
}
```

6. SessionManager가 구현되어 있으며, 싱글톤으로 존재. manager를 통해 BoradCasting 가능

### 추후 구현할 기능들
- 특수한 Memory 구현은 아직 미구현 상태, 추후에 할 예정 (Memory Pool, shared_ptr Custom)
- JobQueue는 구현되어 있으나 JobQueue에 특정 시간을 넣는 기능인 JobTimer 미구현, 구현하는 중
