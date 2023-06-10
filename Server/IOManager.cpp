#include "stdafx.h"

#include "IOManager.hpp"
#include "Session.hpp"
#include "Engine.hpp"
#include "RioContext.hpp"

#include <thread>

thread_local int LThreadId = -1;

RIO_CQ IOManager::mComplQue[MAX_THREAD];

IOManager::IOManager() = default;

void IOManager::Start()
{
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		DWORD dw = 0;
		HANDLE hThrd = (HANDLE)::_beginthreadex(nullptr, 0, IoWorkerThread, (void*)i, 0, (unsigned*)dw);
		ASSERT_CRASH(std::runtime_error, hThrd != NULL);
	}
}

RIO_CQ IOManager::GetCQ(size_t threadId)
{
	return mComplQue[threadId];
}

unsigned int CALLBACK IOManager::IoWorkerThread(LPVOID lpParam)
{
	LThreadId = reinterpret_cast<int>(lpParam);

	auto& cq = mComplQue[LThreadId];
	cq = RIO.RIOCreateCompletionQueue(MAX_CQ_SIZE, NULL);
	if (cq == RIO_INVALID_CQ)
	{
		PrintException(L"Invalid RIO_CQ");
		return -1;
	}
	
	ASSERT_CRASH(net_exception, RIO.RIONotify(cq) != ERROR_SUCCESS);

	RIORESULT rioResult[MAX_RIORESULT];
	while (true)
	{
		ZeroMemory(&rioResult, sizeof(rioResult));

		ULONG numOfResults = RIO.RIODequeueCompletion(mComplQue[LThreadId], rioResult, MAX_RIORESULT);
		if (numOfResults == RIO_CORRUPT_CQ)
		{
			CRASH(net_exception);
		}
		else if (numOfResults == 0) Sleep(1);

		for (ULONG i = 0; i < numOfResults; ++i)
		{
			auto* context = reinterpret_cast<RioContext*>(rioResult[i].RequestContext);
			auto client = context->session;
			auto transferred = rioResult[i].BytesTransferred;

			switch (context->eventType)
			{
			case EventType::Recv:
				client->CompleteRecv(static_cast<RecvContext*>(context), transferred);
				break;
			case EventType::Send:
				client->CompleteSend(static_cast<SendContext*>(context), transferred);
				break;
			default:
				CRASH(net_exception);
				break;
			}
			context->session = nullptr;
		}
	}
}
