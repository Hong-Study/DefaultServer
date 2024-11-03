#include "pch.h"
#include "IOCompletionPort.h"

bool IOCompletionPort::InitSocket()
{
	WSADATA wsaData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	//연결지향형 TCP , Overlapped I/O 소켓을 생성
	_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == _ListenSocket)
	{
		printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("소켓 초기화 성공\n");
	return true;
}

bool IOCompletionPort::BindandListen(short nBindPort, int acceptIp)
{
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(nBindPort);
	sockAddr.sin_addr.s_addr = htonl(acceptIp);

	int ret = bind(_ListenSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if (SOCKET_ERROR == ret)
	{
		printf("[에러] bind()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	ret = listen(_ListenSocket, 5);
	if (SOCKET_ERROR == ret)
	{
		printf("[에러] listen()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("Bind and Listen 성공\n");
	return true;
}

bool IOCompletionPort::StartServer(const UINT32 maxClientCount)
{
	CreateClient(maxClientCount);

	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKTHREAD);
	if (NULL == _IOCPHandle)
	{
		printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	bool bRet = CreateWokerThread();
	if (false == bRet) {
		return false;
	}

	bRet = CreateAcceptThread();
	if (false == bRet) {
		return false;
	}

	printf("서버 시작\n");

	return true;
}

void IOCompletionPort::EndServer()
{
	_IsWorkerRun = false;
	CloseHandle(_IOCPHandle);

	for (auto& th : _IOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	//Accepter 쓰레드를 종요한다.
	_IsAccepterRun = false;
	closesocket(_ListenSocket);

	if (_AccepterThread.joinable())
	{
		_AccepterThread.join();
	}
}

bool IOCompletionPort::SendMsg(stClientInfo* clientInfo, char* msg, int len)
{
	DWORD dwRecvNumBytes = 0;

	// 전송될 메시지를 복사
	CopyMemory(clientInfo->mSendBuf, msg, len);
	clientInfo->mSendBuf[len] = '\0';

	clientInfo->m_stSendOverlappedEx.m_wsaBuf.len = len;
	clientInfo->m_stSendOverlappedEx.m_wsaBuf.buf = clientInfo->mSendBuf;
	clientInfo->m_stSendOverlappedEx.m_eOperation = IOOperation::SEND;

	int nRet = WSASend(
		clientInfo->m_socketClient,
		&(clientInfo->m_stSendOverlappedEx.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (clientInfo->m_stSendOverlappedEx),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

void IOCompletionPort::CreateClient(const UINT32 maxClientCount)
{
	for (UINT32 i = 0; i < maxClientCount; ++i)
	{
		_ClientInfos.emplace_back();
	}
}

void IOCompletionPort::DestoryThread()
{
	_IsWorkerRun = false;
	CloseHandle(_IOCPHandle);

	for (auto& th : _IOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	//Accepter 쓰레드를 종요한다.
	_IsAccepterRun = false;
	closesocket(_ListenSocket);

	if (_AccepterThread.joinable())
	{
		_AccepterThread.join();
	}
}

bool IOCompletionPort::CreateWokerThread()
{
	//WaingThread Queue에 대기 상태로 넣을 쓰레드들 생성 권장되는 개수 : (cpu개수 * 2) + 1 
	for (int i = 0; i < MAX_WORKTHREAD; i++)
	{
		_IOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	printf("WokerThread 시작..\n");
	return true;
}

bool IOCompletionPort::CreateAcceptThread()
{
	_AccepterThread = std::thread([this]() { AcceptThread(); });

	printf("AccepterThread 시작..\n");
	return true;
}

bool IOCompletionPort::BindIOCompletionPort(stClientInfo* clientInfo)
{
	auto hIOCP = CreateIoCompletionPort(
		(HANDLE)clientInfo->m_socketClient,
		_IOCPHandle,
		(ULONG_PTR)clientInfo, 0);


	if (NULL == hIOCP || _IOCPHandle != hIOCP)
	{
		printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool IOCompletionPort::BindRecv(stClientInfo* clientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	clientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
	clientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf = clientInfo->mRecvBuf;
	clientInfo->m_stRecvOverlappedEx.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(clientInfo->m_socketClient,
		&(clientInfo->m_stRecvOverlappedEx.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (clientInfo->m_stRecvOverlappedEx),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void IOCompletionPort::AccpterThread()
{
	SOCKADDR_IN sockAddr;
	int addrLen = sizeof(sockAddr);

	while (_IsAccepterRun)
	{
		stClientInfo* clientInfo = GetEmptyClientInfo();
		if (nullptr == clientInfo)
		{
			printf("[에러] Client Full\n");
			return;;
		}

		clientInfo->m_socketClient = accept(_ListenSocket, (SOCKADDR*)&sockAddr, &addrLen);
		if (INVALID_SOCKET == clientInfo->m_socketClient)
		{
			continue;
		}

		//I/O Completion Port객체와 소켓을 연결시킨다.
		bool bRet = BindIOCompletionPort(clientInfo);
		if (false == bRet)
		{
			return;
		}

		//Recv Overlapped I/O작업을 요청해 놓는다.
		bRet = BindRecv(clientInfo);
		if (false == bRet)
		{
			return;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(sockAddr.sin_addr), clientIP, 32 - 1);
		printf("클라이언트 접속 : IP(%s) SOCKET(%d)\n", clientIP, (int)clientInfo->m_socketClient);

		//클라이언트 갯수 증가
		++_ClientCnt;
	}
}

void IOCompletionPort::CloseSocket(stClientInfo* clientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(clientInfo->m_socketClient, SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(clientInfo->m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//소켓 연결을 종료 시킨다. 
	closesocket(clientInfo->m_socketClient);

	clientInfo->m_socketClient = INVALID_SOCKET;
}

stClientInfo* IOCompletionPort::GetEmptyClientInfo()
{
	for (auto& client : _ClientInfos)
	{
		if (INVALID_SOCKET == client->m_socketClient)
		{
			return client;
		}
	}

	return nullptr;
}

void IOCompletionPort::WokerThread()
{
	//CompletionKey를 받을 포인터 변수
	stClientInfo* clientInfo = NULL;
	//함수 호출 성공 여부
	BOOL bSuccess = TRUE;
	//Overlapped I/O작업에서 전송된 데이터 크기
	DWORD dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while (_IsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(
			_IOCPHandle,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&clientInfo,	// Overlapped IO 객체
			&lpOverlapped,
			INFINITE);

		//사용자 쓰레드 종료 메세지 처리..
		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			_IsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client가 접속을 끊었을때..			
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) 접속 끊김\n", (int)clientInfo->m_socketClient);
			CloseSocket(clientInfo);
			continue;
		}

		stOverlappedEx* overlappedEx = reinterpret_cast<stOverlappedEx*>(lpOverlapped);

		//Overlapped I/O Recv작업 결과 뒤 처리
		if (IOOperation::RECV == overlappedEx->m_eOperation)
		{
			clientInfo->mRecvBuf[dwIoSize] = NULL;
			printf("[수신] bytes : %d , msg : %s\n", dwIoSize, clientInfo->mRecvBuf);

			//클라이언트에 메세지를 에코한다.
			SendMsg(clientInfo, clientInfo->mRecvBuf, dwIoSize);
			BindRecv(clientInfo);
		}
		else if (IOOperation::SEND == overlappedEx->m_eOperation)
		{
			printf("[송신] bytes : %d , msg : %s\n", dwIoSize, clientInfo->mSendBuf);
		}
		else
		{
			printf("socket(%d)에서 예외상황\n", (int)clientInfo->m_socketClient);
		}
	}
}

void IOCompletionPort::AcceptThread()
{
}
