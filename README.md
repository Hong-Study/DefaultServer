# DefaultServer
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
## 용도
- IOCP Server를 위해 사용
- 특수한 Memory 구현은 아직 미구현 상태, 추후에 할 예정 (Memory Pool, shared_ptr Custom)
- SessionManager가 구현되어 있으며, 싱글톤으로 존재. manager를 통해 BoradCasting 가능

## 사용 방식
1. ServerService 를 통해, Session 형태와 NetAddress 를 집어넣음
ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777), 10, std::function<SessionRef()>(make_shared<ClinetSession>));

2. service->Start() 실행
3. 스레드를 만들어 service->GetIocpCore()->Dispatch() 실행, 단 시간 초를 정하고 싶으면 Dispatch의 인자로 넘겨주면 됨.
4. GThreadManager->Join(); 을 통해 대기
5. Recv를 받으면 PacketHandler를 통해 타입에 맞는 함수 실행
6. 단, Protocolbuffer에 구현된 패킷 형태의 함수를 맞춰서 집어넣어 줘야함. ( 자세한 코드는 PacketHandler안에 존재)

---
# C++ Select Server
## 용도
- IOCP를 사용할 필요가 없을 정도로 작은 규모에서 사용하기 위해 제작
- 아직 에코 테스트 정도만 하고, 실제 데이터는 테스트 X

## 사용 방식
1. 그냥 쓰면 됨.
