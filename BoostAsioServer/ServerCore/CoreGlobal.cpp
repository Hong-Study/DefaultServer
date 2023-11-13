#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;
boost::asio::io_context io_context;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		io_context;
	}
	~CoreGlobal()
	{
		delete GThreadManager;
	}
}CoreGlobal;
