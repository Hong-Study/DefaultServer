#pragma once
#include "JobQueue.h"

class TempClass : public JobQueue
{
public:
	TempClass() { }

	void Print() { std::cout << n << std::endl; }
private:
	int n = 10;
};
