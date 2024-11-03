#pragma once
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <memory>
#include <vector>
#include <algorithm>

#define MAX_SOCKBUF 1024
#define MAX_WORKTHREAD 4

enum class IOOperation
{
	RECV,
	SEND,
};

//WSAOVERLAPPED����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� �־���.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O����ü
	SOCKET		m_socketClient;			//Ŭ���̾�Ʈ ����
	WSABUF		m_wsaBuf;				//Overlapped I/O�۾� ����
	IOOperation m_eOperation;			//�۾� ���� ����
};

//Ŭ���̾�Ʈ ������ ������� ����ü
struct stClientInfo
{
	SOCKET			m_socketClient;			//Cliet�� ����Ǵ� ����
	stOverlappedEx	m_stRecvOverlappedEx;	//RECV Overlapped I/O�۾��� ���� ����
	stOverlappedEx	m_stSendOverlappedEx;	//SEND Overlapped I/O�۾��� ���� ����
	
	char			mRecvBuf[MAX_SOCKBUF]; //������ ����
	char			mSendBuf[MAX_SOCKBUF]; //������ ����

	stClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&mRecvBuf, sizeof(MAX_SOCKBUF));
		ZeroMemory(&mSendBuf, sizeof(MAX_SOCKBUF));
		m_socketClient = INVALID_SOCKET;
	}
};