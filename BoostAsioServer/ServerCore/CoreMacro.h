#pragma once	
#include <memory>

#define SHARED_PTR(classname) std::shared_ptr<class classname>
//using JobRef = SHARED_PTR(Job);
//using JobTimeRef = SHARED_PTR(JobTimer);
//using JobQueueRef = SHARED_PTR(JobQueue);

using SessionRef = SHARED_PTR(Session);
using SendBufferRef = SHARED_PTR(SendBuffer);
using PacketSessionRef = SHARED_PTR(PacketSession);
using ServerServiceRef = SHARED_PTR(ServerService);
using ClientServiceRef = SHARED_PTR(ClientService);