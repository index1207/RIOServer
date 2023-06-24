#include "stdafx.h"
#include "RecvBuffer.hpp"

RecvBuffer::RecvBuffer(byte* buffer, size_t bufferSize)
{
	_buffer = buffer;
	_size = bufferSize;
	_readIdx = _writeIdx = 0;
}