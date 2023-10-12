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

## ��� ���
1. ServerService �� ����, Session ���¿� NetAddress �� �������
ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared<ClinetSession>));

2. service->Start() ����
3. �����带 ����� service->GetIocpCore()->Dispatch() ����, �� �ð� �ʸ� ���ϰ� ������ Dispatch�� ���ڷ� �Ѱ��ָ� ��.
4. GThreadManager->Join(); �� ���� ���
5. Recv�� ������ PacketHandler�� ���� Ÿ�Կ� �´� �Լ� ����
6. ��, Protocolbuffer�� ������ ��Ŷ ������ �Լ��� ���缭 ����־� �����. ( �ڼ��� �ڵ�� PacketHandler�ȿ� ����)
7. SessionManager�� �����Ǿ� ������, �̱������� ����. manager�� ���� BoradCasting ����

### ���� ������ ��ɵ�
- Ư���� Memory ������ ���� �̱��� ����, ���Ŀ� �� ���� (Memory Pool, shared_ptr Custom)

- JobQueue�� �����Ǿ� ������ JobQueue�� Ư�� �ð��� �ִ� ��� �̱���, ������ ����

---
# C++ Select Server
## �뵵
- IOCP�� ����� �ʿ䰡 ���� ������ ���� �Ը𿡼� ����ϱ� ���� ����
- ���� ���� �׽�Ʈ ������ �ϰ�, ���� �����ʹ� �׽�Ʈ X
- ���� ��Ʈ������ �����ߴ� �������� �����丵 ����
