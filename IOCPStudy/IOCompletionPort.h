#pragma once
#include <thread>

class IOCompletionPort
{
public:
	IOCompletionPort(void) {}

	~IOCompletionPort(void)
	{
		//윈속의 사용을 끝낸다.
		WSACleanup();
	}

public:
	bool InitSocket();
	bool BindandListen(short nBindPort, int acceptIp = INADDR_ANY);
	bool StartServer(const UINT32 maxClientCount);
	void EndServer();

	bool SendMsg(stClientInfo* clientInfo, char* msg, int len);

private:
	void CreateClient(const UINT32 maxClientCount);
	void DestoryThread();

	// IOCP Port에서 발생한 이벤트 처리용 스레드
	bool CreateWokerThread();
	bool CreateAcceptThread();

	bool BindIOCompletionPort(stClientInfo* clientInfo);

	// IOCP Port에 Send, Recv를 등록한다.
	bool BindRecv(stClientInfo* clientInfo);

	// 사용자의 접속을 받는 쓰레드
	void AccpterThread();

	void CloseSocket(stClientInfo* clientInfo, bool bIsForce = false);
	
	stClientInfo* GetEmptyClientInfo();

private:
	void WokerThread();
	void AcceptThread();

private:
	//클라이언트 정보 저장 구조체
	std::vector<stClientInfo*> _ClientInfos;

	//클라이언트의 접속을 받기위한 리슨 소켓
	SOCKET		_ListenSocket = INVALID_SOCKET;

	//접속 되어있는 클라이언트 수
	int			_ClientCnt = 0;

	//IO Worker 스레드
	std::vector<std::thread> _IOWorkerThreads;

	//Accept 스레드
	std::thread	_AccepterThread;

	//CompletionPort객체 핸들
	HANDLE		_IOCPHandle = INVALID_HANDLE_VALUE;

	//작업 쓰레드 동작 플래그
	bool		_IsWorkerRun = true;

	//접속 쓰레드 동작 플래그
	bool		_IsAccepterRun = true;

	//소켓 버퍼
	char		_SocketBuf[1024] = { 0, };
};

