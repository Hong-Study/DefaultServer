#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(uint64 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(uint64 numOfBytes);
	bool			OnWrite(uint64 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	uint64			DataSize() { return _writePos - _readPos; }
	uint64			FreeSize() { return _capacity - _writePos; }

private:
	uint64				_capacity = 0;
	uint64				_bufferSize = 0;
	uint64				_readPos = 0;
	uint64				_writePos = 0;
	std::vector<BYTE>	_buffer;
};

