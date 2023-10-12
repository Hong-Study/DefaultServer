#pragma once

struct JobData
{
	JobData(JobQueueRef owner, JobRef job, uint64 time) : owner(owner), job(job), time(time) { }
	
	bool				operator<(uint64 currentTick) { return time < currentTick; }

	weak_ptr<JobQueue>	owner;
	JobRef				job;
	uint64				time;
};

class JobTimer
{
public:
	void			Reserve(JobQueueRef owner, JobRef job, uint64 tickAfter);
	void			Distribute(uint64 now);
	void			Clear();

private:
	LockQueue<JobData*>	_jobDatas;
	atomic<bool>		_isDistribute = false;
};

