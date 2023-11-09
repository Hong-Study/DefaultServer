#include "pch.h"
#include "Service.h"

Service::Service(std::string ip, int32 port, SessionFunction function)
	: _endPoint(boost::asio::ip::make_address(ip), port), _sessionFunction(function)
{

}

Service::~Service()
{

}

ServerService::ServerService(std::string ip, int32 port, int32 accepterCount, SessionFunction function)
	: Service(ip, port, function), _accepterCount(accepterCount)
{

}

ServerService::~ServerService()
{
	_accepters.clear();
}

bool ServerService::Start()
{
	for (int i = 0; i < _accepterCount; i++)
	{
		std::shared_ptr<Accepter> accepter = make_shared<Accepter>(_ioContext, _endPoint, _sessionFunction);
		accepter->StartAccept();

		_accepters.emplace_back(accepter);
	}

	return true;
}

ClientService::ClientService(std::string ip, int32 port, SessionFunction function)
	: Service(ip, port, function)
{

}

ClientService::~ClientService()
{

}

bool ClientService::Start()
{
	return false;
}
