#pragma once

class TestManager : public JobQueue
{
public:
	void Send(string prints);
	void SendByNow(string str);
};

extern shared_ptr<TestManager> Gtest;

