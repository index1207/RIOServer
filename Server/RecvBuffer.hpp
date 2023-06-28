#pragma once
//5#include "RioBuffer.hpp"

class RecvBuffer
{
	friend class RioBuffer;
public:
	RecvBuffer(size_t pageCount);
	~RecvBuffer();

	auto inline GetBuffer() -> byte* const
	{
		return _buffer;
	}

	auto inline GetDataSize() -> size_t
	{
		return _writeIdx - _readIdx;
	}
	auto inline GetFreeSize() -> size_t
	{
		return _pageCount * 0x1000 - _writeIdx;
	}

	auto inline GetReadOffset() -> size_t
	{
		return _readIdx;
	}
	auto inline GetWriteOffset() -> size_t
	{
		return _writeIdx;
	}

	auto Clear() -> void
	{
		if (GetDataSize() == 0)
		{
			_readIdx = 0;
			_writeIdx = 0;
		}
		else
		{
			std::copy(_buffer, _buffer + _writeIdx, _buffer);
			_readIdx = 0;
			_writeIdx = GetDataSize();
		}
	}

	auto inline OnRead(int numOfBytes) -> bool
	{
		if (numOfBytes > GetDataSize())
			return false;
		_readIdx += numOfBytes;
		return true;
	}
	auto inline OnWrite(int numOfBytes) -> bool
	{
		if (numOfBytes > GetFreeSize())
			return false;
		_writeIdx += numOfBytes;
		return true;
	}
private:
	byte* _buffer;
	size_t _pageCount;
	size_t _readIdx;
	size_t _writeIdx;
};

