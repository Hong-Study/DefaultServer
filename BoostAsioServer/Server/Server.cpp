// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "Service.h"
#include "ClientSession.h"

SessionRef MakeSession(boost::asio::io_context& context)
{
    return std::make_shared<ClientSession>(context);
}

int main()
{
    ServerService service("127.0.0.1", 4242, 5, MakeSession);

    if (service.Start())
    {
        std::cout << "Start Success" << std::endl;

        service.Run();
    }
}