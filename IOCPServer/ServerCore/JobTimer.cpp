#include "pch.h"
#include "JobTimer.h"

void JobTimer::Reserve(JobQueueRef owner, JobRef job, uint64 tickAfter)
{
	uint64 time = GetTickCount64() + tickAfter;

	_jobDatas.Push(new JobData{ owner, job, time });
}

void JobTimer::Distribute(uint64 now)
{
	bool expected = false;
	if (_isDistribute.compare_exchange_strong(expected, true) == false)
		return;

	vector<JobData*> jobs;
	if (_jobDatas.PopAll(jobs) == false)
	{
		_isDistribute.store(false);
		return;
	}
}

void JobTimer::Clear()
{
	
}
