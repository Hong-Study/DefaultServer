#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 size)
	: _bufferSize(size)
{
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_writePos = _readPos = 0;
	}
	else if(FreeSize() < _bufferSize)
	{
		::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
		_readPos = 0;
		_writePos = dataSize;
	}
}

bool RecvBuffer::OnRead(const int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(const int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;
	
	_writePos += numOfBytes;
	return true;
}