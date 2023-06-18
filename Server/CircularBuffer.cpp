#include "stdafx.h"
#include "CircularBuffer.hpp"

CircularBuffer::CircularBuffer(byte* buffer, size_t size)
	: buffer(buffer), maxSize(size), tail(0), head(0)
{
}
	
bool CircularBuffer::isEmpty() const
{
	return tail == head;
}

size_t CircularBuffer::getFreeSpace()
{
	return maxSize-head;
}

size_t CircularBuffer::getWritableOffset()
{
	return tail;
}

size_t CircularBuffer::getReadableOffset()
{
	return head;
}

void CircularBuffer::enque(size_t length)
{
	tail = (tail + length) % maxSize;
}

void CircularBuffer::deque(size_t length)
{
	head = (head + length) % maxSize;
}
