#include "pch.h"
#include "TestManager.h"

shared_ptr<TestManager> Gtest = make_shared<TestManager>();

void TestManager::Send(PacketSessionRef ref, SendBufferRef sendBuf)
{
	ref->Send(sendBuf);
}


