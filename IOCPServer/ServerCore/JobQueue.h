#pragma once
#include "LockQueue.h"

using CallBack = std::function<void()>;

class Job
{
public:	
	Job() { }
	Job(CallBack&& func) : _func(std::move(func)) { }
	
	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> obj, Ret(T::* memfunc)(Args...), Args... args) 
	{  
		_func = [obj, memfunc, args...]() {
			(obj.get()->*memfunc)(args...);
		};
	}

	void Excute() { _func(); }

private:
	CallBack _func;
};

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallBack&& func)
	{
		JobRef job = make_shared<Job>(std::move(func));
		Push(job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::*memFunc)(Args...), Args... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<Job>(ptr, memFunc, std::forward<Args>(args)...);
		Push(job);
	}

	/*void DoTimer(CallBack&& func, uint64 time)
	{
		JobRef job = make_shared<Job>(std::move(func));
		JobTimeRef jobTime = make_shared<JobTimer>(job, time);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<Job>(ptr, memFunc, std::forward<Args>(args)...);
		JobTimeRef jobTime = make_shared<JobTimer>(job, time);
	}*/

public:
	void Excute();
	void Push(JobRef job);

private:
	USE_LOCK;
	LockQueue<JobRef> _queue;
	atomic<int32> _insideQueue = 0;
	atomic<bool> _isExcute = false; 
};

