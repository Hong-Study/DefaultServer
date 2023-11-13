#pragma once

// ��� ���񽺸� �־��� ���ΰ�
using CallBack = std::function<void()>;

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
protected:
	JobQueue();
	~JobQueue();

public:
	void DoAsync(CallBack&& func)
	{
		// �ʱ�ȭ
		if (_strand == nullptr)
			_strand = new boost::asio::io_context::strand(io_context);

		boost::asio::post(boost::asio::bind_executor(*_strand, [func]() {
			func();
			}));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		// �ʱ�ȭ
		if (_strand == nullptr)
			_strand = new boost::asio::io_context::strand(io_context);

		std::shared_ptr<T> obj = std::static_pointer_cast<T>(shared_from_this());
		boost::asio::post(boost::asio::bind_executor(*_strand, [obj, memFunc, args...]() {
			(obj.get()->*memFunc)(args...);
			}));
	}

private:
	// JobQueue�� �׾ strand�� �����ɱ�?
	boost::asio::io_context::strand* _strand = nullptr;
};

