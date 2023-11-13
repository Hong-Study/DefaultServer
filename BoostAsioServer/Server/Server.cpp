// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "Service.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include "JobQueue.h"
#include "TempClass.h"

SessionRef MakeSession(boost::asio::io_context& context)
{
    return std::make_shared<ClientSession>(context);
}

std::shared_ptr<TempClass> tempPtr = std::make_shared<TempClass>();

int main()
{
    ServerService service("127.0.0.1", 4242, 5, MakeSession);

   
    //tempPtr->DoAsync(&TempClass::Print);

    if (service.Start())
    {
        std::cout << "Start Success" << std::endl;

        for (int i = 0; i < 10; i++)
        {
            GThreadManager->Launch([&service]() {
                while (true)
                {
                    service.Run();
                }
                });
        }
    }

    service.Run();

    GThreadManager->Join();
}