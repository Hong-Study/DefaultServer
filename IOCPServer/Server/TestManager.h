#pragma once
class TestManager : public JobQueue
{
public:
	void Send(PacketSessionRef ref, SendBufferRef sendBuf);
};

extern shared_ptr<TestManager> Gtest;

