#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef job)
{
	int32 size = _insideQueue.fetch_add(1);
	_queue.Push(job);

	if (size == 0) {
		Excute();
	}
}

void JobQueue::Excute()
{
	bool expected = false;
	if (_isExcute.compare_exchange_strong(expected, true))
	{
		int64 startTick = GetTickCount64();

		while (true)
		{
			JobRef job = Pop();
			if (job == nullptr)
				break;
			
			job->Excute();

			int64 endTick = GetTickCount64();
			
			if (endTick - startTick > 1000)
			{
				GQUEUEE->Push(shared_from_this());

				break;
			}
		}

		_isExcute.store(false);
	}
}
