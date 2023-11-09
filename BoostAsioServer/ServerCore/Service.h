#pragma once
#include "Accepter.h"

class Service
{
public:
	Service(std::string ip, int32 port, SessionFunction function);
	virtual ~Service();

	virtual bool Start() abstract;
	void Run() { _ioContext.run(); }

protected:
	boost::asio::io_context _ioContext;
	boost::asio::ip::tcp::endpoint _endPoint;

	SessionFunction _sessionFunction;
};

class ServerService : public Service
{
public:
	ServerService(std::string ip, int32 port, int32 accepterCount, SessionFunction function);
	virtual ~ServerService();

	virtual bool Start() override;

private:
	int32 _accepterCount;
	std::vector<std::shared_ptr<Accepter>> _accepters;
};

class ClientService : public Service
{
public:
	ClientService(std::string ip, int32 port, SessionFunction function);
	virtual ~ClientService();

	virtual bool Start() override;
};