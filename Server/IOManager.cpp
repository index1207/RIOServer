#include "stdafx.h"
#include "IOManager.hpp"

#include "Engine.hpp"

#include <thread>

thread_local int LThreadId = -1;

RIO_CQ IOManager::mRioCq[MAX_THREAD];

IOManager::IOManager() = default;

void IOManager::Start()
{
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		DWORD dw = 0;
		HANDLE hThrd = (HANDLE)::_beginthreadex(nullptr, 0, IoWorkerThread, (void*)i, 0, (unsigned*)dw);
		if (hThrd == NULL)
		{
			throw std::exception();
		}
	}
}

unsigned int CALLBACK IOManager::IoWorkerThread(LPVOID lpParam)
{
	LThreadId = reinterpret_cast<int>(lpParam);

	mRioCq[LThreadId] = RIO.RIOCreateCompletionQueue(MAX_CQ_SIZE, NULL);
	if (mRioCq[LThreadId] == RIO_INVALID_CQ)
	{
		throw network_error();
	}

	RIORESULT rioResult[MAX_RIORESULT];
	while (true)
	{
		ZeroMemory(&rioResult, sizeof(rioResult));

		ULONG numOfResults = RIO.RIODequeueCompletion(mRioCq[LThreadId], rioResult, MAX_RIORESULT);
		if (numOfResults == RIO_CORRUPT_CQ)
		{
			throw network_error();
		}
	}
}
