#include "pch.h"
#include "JobQueue.h"
#include "Service.h"

JobQueue::JobQueue()
	: _strand(nullptr)
{

}

JobQueue::~JobQueue()
{
	if (_strand != nullptr)
	{
		if(_strand->running_in_this_thread())
			_strand->on_work_finished();

		delete _strand;
	}
}
