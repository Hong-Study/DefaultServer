#pragma once

#define USE_LOCK std::mutex _mutex
#define WRITE_LOCK std::lock_guard<std::mutex> lock_guard(_mutex)

template<typename T>
class LockQueue
{
public:
	void Push(T data)
	{
		WRITE_LOCK;
		_queue.push(data);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (_queue.empty())
			return nullptr;
		T data = _queue.front();
		_queue.pop();

		return data;
	}

	bool PopAll(std::vector<T>& datas)
	{
		WRITE_LOCK;
		if (_queue.empty())
			return false;

		for (int i = 0; i < _queue.size(); i++)
		{
			_queue.front();
			datas.push_back(_queue.front());
			_queue.pop();
		}

		return true;
	}

	int Size()
	{
		WRITE_LOCK;
		return _queue.size();
	}

private:
	USE_LOCK;
	std::queue<T> _queue;
};

