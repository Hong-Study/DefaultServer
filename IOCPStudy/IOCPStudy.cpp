#include "pch.h"
#include "IOCompletionPort.h"
#include <iostream>
#include <string>

int main()
{
	IOCompletionPort iocp;
	
	// 소켓 초기화
	iocp.InitSocket();

	// 서버 주소와 연결하고 등록 시킨다.
	iocp.BindandListen(9000);

	// 시작
	iocp.StartServer(100);

	printf("아무 키나 누를 때까지 대기합니다\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	iocp.EndServer();
}