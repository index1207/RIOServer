#include "stdafx.h"
#include "RecvBuffer.hpp"
#include "RioBuffer.hpp"

RecvBuffer::RecvBuffer(size_t pageCount)
{
	_buffer = reinterpret_cast<byte*>(::VirtualAllocEx(GetCurrentProcess(), 0, RioBuffer::PAGE_SIZE * pageCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	_pageCount = pageCount;
	_readIdx = _writeIdx = 0;
}

RecvBuffer::~RecvBuffer()
{
	if (_buffer)
		::VirtualFreeEx(GetCurrentProcess(), _buffer, 0, MEM_RELEASE);
}
