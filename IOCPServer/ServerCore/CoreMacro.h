#pragma once
#include <iostream>
#include <memory>

#define USE_LOCK Lock _lock
#define USE_MANY_LOCK(i) Lock _lock[i]
#define WRITE_LOCK WriteLockGuard _guard(_lock, typeid(this).name())
#define WRITE_MANY_LOCK(i) WriteLockGuard _guard##i(_lock[i], typeid(this).name())
#define READ_LOCK ReadLockGuard _guard(_lock, typeid(this).name())
#define READ_MANY_LOCk(i) ReadLockGuard _guard##i(_lock[i], typeid(this).name())

#define SESSIONS SessionManager::GetInstance()
#define GQUEUEE GlobalQueue::GetInstance()

#define CRASH(cause)						\
{											\
	cout << cause <<endl;					\
}
//	uint32* crash = nullptr;				\
//	__analysis_assume(crash != nullptr);	\
//	*crash = 0xDEADBEEF;					\
//}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

using JobRef = std::shared_ptr<class Job>;
using JobQueueRef = std::shared_ptr<class JobQueue>;

using IocpObjectRef = std::shared_ptr<class IocpObject>;
using SessionRef = std::shared_ptr<class Session>;
using SendBufferRef = std::shared_ptr<class SendBuffer>;
using PacketSessionRef = std::shared_ptr<class PacketSession>;
using ListenerRef = std::shared_ptr<class Listener>;
using IocpCoreRef = std::shared_ptr<class IocpCore>;
using ServerServiceRef = std::shared_ptr<class ServerService>;
using ClientServiceRef = std::shared_ptr<class ClientService>;