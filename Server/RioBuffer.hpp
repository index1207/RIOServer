#pragma once
#include "Rio.hpp"
#include "RecvBuffer.hpp"

class RioBuffer
{
public:
	enum { PAGE_SIZE = 0x1000 };
public:
	static RIO_BUFFERID RegisterBuffer(RecvBuffer* recvBuf)
	{
		return RIO.RIORegisterBuffer(reinterpret_cast<PCHAR>(recvBuf->_buffer), recvBuf->_pageCount * PAGE_SIZE);
	}
	static void DeregisterBuffer(RIO_BUFFERID rioBufId)
	{
		RIO.RIODeregisterBuffer(rioBufId);
	}
};

