# DefaultServer
### 제작 이유
- 스마일게이트에서 팀 프로젝트를 진행하면서 부족했던 점을 보완하기 위해 제작하였습니다.
  
[스마일게이트 팀 프로젝트](https://github.com/Hong-Study/bluebird/tree/main/src)
- 추가적으로 서버 공부를 위해서 제작하였습니다.

### 개발 기간
2023.05 ~ 개발 중

### 설명
- 게임 개발시 서버 코어 제작하는 기간을 줄이기 위한 기본 토대 서버입니다.
- 패킷 통신은 Protobuf를 사용하여 통신 규약을 통일화한다는 전제 조건으로 구현되었습니다.

# Cshap Client
## 용도
- Unity 또는 C# 프로그램에서 C++ 서버와 통신을 위해 사용합니다.
- 또한 서로 다른 언어의 통신을 위해 ProtocolBuf를 사용합니다. 따라서 visual studio 에서 Protobuf를 추가해주어야 합니다.
- 통신 방식은 APM 방식의 비동기 통신을 사용합니다.

## 사용 방식
1. NetworkManager.Instance.Start() 호출
2. Connect 가 실행 된 이후 자동으로 Recv 호출
3. Recv가 호출되면 PacketHeader를 통해 패킷 형태 확인
4. PacketHandler에서 type에 맞는 함수 실행
5. Send시에는 NetworkManager.Instance.Send(IMessage)를 실행

---
# C++ IOCP Server
- C++ 17으로 제작

### 용도
- IOCP Server를 위해 사용

### 자세한 설명
![IOCP Server](https://github.com/Hong-Study/DefaultServer/tree/main/IOCPServer)

# C++ Boost::Asio 서버
- Boost::Asio 강의를 듣고 기존의 IOCP 서버를 토대로 코어를 변경해보았습니다.
- 실제로 IOCP 서버의 코드보다 간결해졌으며 구현에 있어서도 구현에 있어서도 쉬울 수 있었습니다.
- 또한 Boost::asio는 리눅스, 윈도우 모두에서 작동이 가능하므로 서버 구분 없이도 구현이 가능하다는 장점이 있습니다.

### 자세한 설명
![Boost::Asio Server](https://github.com/Hong-Study/DefaultServer/tree/main/BoostAsioServer)

# PacketGenerator
- C# 으로 제작된 PacketHandler.h 자동 구현 툴
- Enum.proto의 enum INGAME을 읽어서 Client용 PacketHandler와 Server용 PacketHandler를 자동으로 구현해줍니다.
- Boost::Asio, IOCP 서버에 들어가 있습니다.

```
// ../../../ = PacketGenerator
ReadWriteFile readWriteFile = new ReadWriteFile("../../../../Common/protoc-21.12-win64/bin/Enum.proto");

bool isOnlyCpp = true;
if (isOnlyCpp) // C++ Client - Server
{
	string serverPath = "../../../../GameServer/ServerPacketHandler.h";
	string clientPath = "../../../../../Capstone/Source/Capstone/ClientPacketHandler.h";
	
	readWriteFile.MakeOnlyCppHandler(serverPath, clientPath);
}
else // C# Client - Server
{
	string serverPath = "../../../../Server/ServerPacketHandler.h";
	string clientPath = "../../../../../CsharpClient/GameServer/Packet/";
	
	readWriteFile.MakeMultiHandler(serverPath, clientPath);
}

{
	string destPath = "../../../../Common/protoc-21.12-win64/bin/Protocol.proto";
	
	readWriteFile.MakeProto(destPath, isOnlyCpp);
}
```

[Enum.proto](Common/protoc-21.12-win64/bin/Enum.proto) 형식
```
enum INGAME
{
	INSIDE = 0;
	OUTSIDE = 1;
	START = 2;
}
```
※ Protocol.proto가 자동으로 빌드되어 파일로 만들어지길 원한다면 빌드전이벤트에 [GenPackets.bat](Common/protoc-21.12-win64/bin/GenPackets.bat) 실행하도록 할 것

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

---
# C++ Select Server
## 용도
- IOCP를 사용할 필요가 없을 정도로 작은 규모에서 사용하기 위해 제작
- 아직 에코 테스트 정도만 하고, 실제 데이터는 테스트 X
- 현재 에트리에서 했던 것을 토대로 리팩토링 예정

# C++ Boost::asio Server
