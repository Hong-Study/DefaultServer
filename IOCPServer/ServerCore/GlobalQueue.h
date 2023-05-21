#pragma once
#include "JobQueue.h"

class GlobalQueue
{
	SINGLETON(GlobalQueue)

public:
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

