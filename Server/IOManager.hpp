#pragma once

extern thread_local int LThreadId;

class IOManager
{
	enum class IO_TYPE
	{
		Send,
		Recv
	};
public:
	IOManager();
public:
	void Start();
private:
	static unsigned CALLBACK IoWorkerThread(LPVOID lpParam);
private:
	static RIO_CQ mRioCq[];
};