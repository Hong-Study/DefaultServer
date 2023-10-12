# DefaultServer
### ���� ����
- �����ϰ���Ʈ���� �� ������Ʈ�� �����ϸ鼭 �����ߴ� ���� �����ϱ� ���� �����Ͽ����ϴ�.
- �߰������� ���� ���θ� ���ؼ� �����Ͽ����ϴ�.

### ����
- ���� ���߽� ���� �ھ� �����ϴ� �Ⱓ�� ���̱� ���� �⺻ ��� �����Դϴ�.
- ��Ŷ ����� Protobuf�� ����Ѵٴ� �����Ϸ� �����Ǿ����ϴ�

# Cshap Client
## �뵵
- Unity �Ǵ� C# ���α׷����� C++ ������ ����� ���� ����մϴ�.
- ���� ���� �ٸ� ����� ����� ���� ProtocolBuf�� ����մϴ�. ���� visual studio ���� Protobuf�� �߰����־�� �մϴ�.
- ��� ����� APM ����� �񵿱� ����� ����մϴ�.

## ��� ���
1. NetworkManager.Instance.Start() ȣ��
2. Connect �� ���� �� ���� �ڵ����� Recv ȣ��
3. Recv�� ȣ��Ǹ� PacketHeader�� ���� ��Ŷ ���� Ȯ��
4. PacketHandler���� type�� �´� �Լ� ����
5. Send�ÿ��� NetworkManager.Instance.Send(IMessage)�� ����

---
# C++ IOCP Server
### �뵵
- IOCP Server�� ���� ���

### PacketGenerator
- C# ���� ���۵� PacketHandler.h �ڵ� ���� ��
- Enum.proto�� enum INGAME�� �о Client�� PacketHandler�� Server�� PacketHandler�� �ڵ����� �������ݴϴ�.
```
static void Main()
{
	//MakeProto(); // Enum.proto �� ���� Protocol.proto�� ��������
	MakeHandle(); // Enum.Proto�� ���� PacketHandler ����
}
```

[Enum.proto](Common/protoc-21.12-win64/bin/Enum.proto) ����
```
enum INGAME
{
	INSIDE = 0;
	OUTSIDE = 1;
	START = 2;
}
```

### ���� ���
1. ServerService �� ����, Session ���¿� NetAddress �� ������� 
```ServerServiceRef service = make_shared(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared));```
2. service->Start() ����
3. �����带 ����� service->GetIocpCore()->Dispatch() ����, �� �ð� �ʸ� ���ϰ� ������ Dispatch�� ���ڷ� �Ѱ��ָ� ��.
4. GThreadManager->Join(); �� ���� ���
```
void DoWork(ServerServiceRef& service)
{
	while (true)
	{
		service->GetIocpCore()->Dispatch(10);

		GThreadManager->DoGlobalQueue();
	}
}

int main()
{
	SocketUtils::Init();
	PacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777)
	, 10, std::function<SessionRef()>(make_shared<ClinetSession>));

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

	GThreadManager->Join();

	// ���� ����
	SocketUtils::Clear();
}
```

5. Recv�� ������ PacketHandler�� ���� Ÿ�Կ� �´� �Լ� ����
��, Protocolbuffer�� ������ ��Ŷ ������ �Լ��� ���缭 ����־� �����.
[Protobuf-Enum.proto](https://github.com/Hong-Study/DefaultServer/tree/main/Common/protoc-21.12-win64/bin)
```
void ClinetSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef ref = GetPacketSessionRef();

	if (PacketHandler::PakcetHandle(ref, buffer, len) == false)
		cout << "Recv Failed" << endl;
}
```

6. SessionManager�� �����Ǿ� ������, �̱������� ����. manager�� ���� BoradCasting ����

### ���� ������ ��ɵ�
- Ư���� Memory ������ ���� �̱��� ����, ���Ŀ� �� ���� (Memory Pool, shared_ptr Custom)
- JobQueue�� �����Ǿ� ������ JobQueue�� Ư�� �ð��� �ִ� ��� �̱���, ������ ����

---
# C++ Select Server
## �뵵
- IOCP�� ����� �ʿ䰡 ���� ������ ���� �Ը𿡼� ����ϱ� ���� ����
- ���� ���� �׽�Ʈ ������ �ϰ�, ���� �����ʹ� �׽�Ʈ X
- ���� ��Ʈ������ �ߴ� ���� ���� �����丵 ����