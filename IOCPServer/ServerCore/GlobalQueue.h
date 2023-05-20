#pragma once
#include "JobQueue.h"

class GlobalQueue
{
private:
	GlobalQueue() { }
	GlobalQueue(const GlobalQueue& ref) { }
	GlobalQueue& operator=(const GlobalQueue& ref) { }
	~GlobalQueue() { }
public:
	static GlobalQueue* GetInstance()
	{
		static GlobalQueue instance;
		return &instance;
	}

	void Push(JobQueueRef ref)
	{
		WRITE_LOCK;
		_jobQueue.push(ref);
	}

	JobQueueRef Pop()
	{
		WRITE_LOCK;
		if (_jobQueue.empty())
			return nullptr;

		JobQueueRef ref = _jobQueue.front();
		_jobQueue.pop();

		return ref;
	}

private:
	USE_LOCK;
	queue<JobQueueRef> _jobQueue;
};

