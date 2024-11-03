#pragma once
#include <thread>

class IOCompletionPort
{
public:
	IOCompletionPort(void) {}

	~IOCompletionPort(void)
	{
		//������ ����� ������.
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

	// IOCP Port���� �߻��� �̺�Ʈ ó���� ������
	bool CreateWokerThread();
	bool CreateAcceptThread();

	bool BindIOCompletionPort(stClientInfo* clientInfo);

	// IOCP Port�� Send, Recv�� ����Ѵ�.
	bool BindRecv(stClientInfo* clientInfo);

	// ������� ������ �޴� ������
	void AccpterThread();

	void CloseSocket(stClientInfo* clientInfo, bool bIsForce = false);
	
	stClientInfo* GetEmptyClientInfo();

private:
	void WokerThread();
	void AcceptThread();

private:
	//Ŭ���̾�Ʈ ���� ���� ����ü
	std::vector<stClientInfo*> _ClientInfos;

	//Ŭ���̾�Ʈ�� ������ �ޱ����� ���� ����
	SOCKET		_ListenSocket = INVALID_SOCKET;

	//���� �Ǿ��ִ� Ŭ���̾�Ʈ ��
	int			_ClientCnt = 0;

	//IO Worker ������
	std::vector<std::thread> _IOWorkerThreads;

	//Accept ������
	std::thread	_AccepterThread;

	//CompletionPort��ü �ڵ�
	HANDLE		_IOCPHandle = INVALID_HANDLE_VALUE;

	//�۾� ������ ���� �÷���
	bool		_IsWorkerRun = true;

	//���� ������ ���� �÷���
	bool		_IsAccepterRun = true;

	//���� ����
	char		_SocketBuf[1024] = { 0, };
};

