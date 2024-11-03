#include "pch.h"
#include "IOCompletionPort.h"

bool IOCompletionPort::InitSocket()
{
	WSADATA wsaData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	//���������� TCP , Overlapped I/O ������ ����
	_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == _ListenSocket)
	{
		printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("���� �ʱ�ȭ ����\n");
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
		printf("[����] bind()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	ret = listen(_ListenSocket, 5);
	if (SOCKET_ERROR == ret)
	{
		printf("[����] listen()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("Bind and Listen ����\n");
	return true;
}

bool IOCompletionPort::StartServer(const UINT32 maxClientCount)
{
	CreateClient(maxClientCount);

	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKTHREAD);
	if (NULL == _IOCPHandle)
	{
		printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", WSAGetLastError());
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

	printf("���� ����\n");

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

	//Accepter �����带 �����Ѵ�.
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

	// ���۵� �޽����� ����
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

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
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

	//Accepter �����带 �����Ѵ�.
	_IsAccepterRun = false;
	closesocket(_ListenSocket);

	if (_AccepterThread.joinable())
	{
		_AccepterThread.join();
	}
}

bool IOCompletionPort::CreateWokerThread()
{
	//WaingThread Queue�� ��� ���·� ���� ������� ���� ����Ǵ� ���� : (cpu���� * 2) + 1 
	for (int i = 0; i < MAX_WORKTHREAD; i++)
	{
		_IOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	printf("WokerThread ����..\n");
	return true;
}

bool IOCompletionPort::CreateAcceptThread()
{
	_AccepterThread = std::thread([this]() { AcceptThread(); });

	printf("AccepterThread ����..\n");
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
		printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool IOCompletionPort::BindRecv(stClientInfo* clientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
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

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
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
			printf("[����] Client Full\n");
			return;;
		}

		clientInfo->m_socketClient = accept(_ListenSocket, (SOCKADDR*)&sockAddr, &addrLen);
		if (INVALID_SOCKET == clientInfo->m_socketClient)
		{
			continue;
		}

		//I/O Completion Port��ü�� ������ �����Ų��.
		bool bRet = BindIOCompletionPort(clientInfo);
		if (false == bRet)
		{
			return;
		}

		//Recv Overlapped I/O�۾��� ��û�� ���´�.
		bRet = BindRecv(clientInfo);
		if (false == bRet)
		{
			return;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(sockAddr.sin_addr), clientIP, 32 - 1);
		printf("Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)\n", clientIP, (int)clientInfo->m_socketClient);

		//Ŭ���̾�Ʈ ���� ����
		++_ClientCnt;
	}
}

void IOCompletionPort::CloseSocket(stClientInfo* clientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

	// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown(clientInfo->m_socketClient, SD_BOTH);

	//���� �ɼ��� �����Ѵ�.
	setsockopt(clientInfo->m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//���� ������ ���� ��Ų��. 
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
	//CompletionKey�� ���� ������ ����
	stClientInfo* clientInfo = NULL;
	//�Լ� ȣ�� ���� ����
	BOOL bSuccess = TRUE;
	//Overlapped I/O�۾����� ���۵� ������ ũ��
	DWORD dwIoSize = 0;
	//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = NULL;

	while (_IsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(
			_IOCPHandle,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&clientInfo,	// Overlapped IO ��ü
			&lpOverlapped,
			INFINITE);

		//����� ������ ���� �޼��� ó��..
		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			_IsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client�� ������ ��������..			
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) ���� ����\n", (int)clientInfo->m_socketClient);
			CloseSocket(clientInfo);
			continue;
		}

		stOverlappedEx* overlappedEx = reinterpret_cast<stOverlappedEx*>(lpOverlapped);

		//Overlapped I/O Recv�۾� ��� �� ó��
		if (IOOperation::RECV == overlappedEx->m_eOperation)
		{
			clientInfo->mRecvBuf[dwIoSize] = NULL;
			printf("[����] bytes : %d , msg : %s\n", dwIoSize, clientInfo->mRecvBuf);

			//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			SendMsg(clientInfo, clientInfo->mRecvBuf, dwIoSize);
			BindRecv(clientInfo);
		}
		else if (IOOperation::SEND == overlappedEx->m_eOperation)
		{
			printf("[�۽�] bytes : %d , msg : %s\n", dwIoSize, clientInfo->mSendBuf);
		}
		else
		{
			printf("socket(%d)���� ���ܻ�Ȳ\n", (int)clientInfo->m_socketClient);
		}
	}
}

void IOCompletionPort::AcceptThread()
{
}
