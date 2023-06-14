#pragma once

class CircularBuffer
{
public:
	CircularBuffer() = default;
	CircularBuffer(byte* buffer, size_t size);
	~CircularBuffer() {}
public:
	bool isEmpty() const;
public:
	size_t getFreeSpace();
	size_t getWritableOffset();
	size_t getReadableOffset();
public:
	void enque(size_t length);
	void deque(size_t length);
private:
	byte* buffer;
	size_t maxSize;
	
	std::atomic<size_t> tail;
	std::atomic<size_t> head;
};

