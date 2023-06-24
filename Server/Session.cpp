#include "stdafx.h"
#include "Session.hpp"

#include "Rio.hpp"
#include "IOManager.hpp"

#include <format>
#include <queue>


Session::Session(int threadId)
	: mThreadId(threadId), mSock(INVALID_SOCKET), mReqQue(RIO_INVALID_RQ), mDisconnected(true)
{
	mBuffer = reinterpret_cast<byte*>(::VirtualAllocEx(GetCurrentProcess(), 0, BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	if (mBuffer == nullptr)
	{
		throw std::runtime_error("Virtual memory allocation error.");
	}

	recvBuffer = new RecvBuffer(mBuffer, BUFFER_SIZE);

	mBufferId = RIO.RIORegisterBuffer(reinterpret_cast<PCHAR>(mBuffer), BUFFER_SIZE);
	if (mBufferId == RIO_INVALID_BUFFERID)
	{
		CRASH(net_exception);
	}
}

Session::~Session()
{
	delete recvBuffer;
	RIO.RIODeregisterBuffer(mBufferId);
	::VirtualFreeEx(GetCurrentProcess(), mBuffer, 0, MEM_RELEASE);
}

void Session::Initialize(SOCKET sock, IPAddress ipAddress)
{
	std::lock_guard<std::mutex> lock(mMtx);

	mSock = sock;
	mIpAddress = ipAddress;

	u_long isBlocking = false;
	::ioctlsocket(mSock, FIONBIO, &isBlocking);

	SocketUtils::setsockopt(mSock, IPPROTO_TCP, TCP_NODELAY, true);

	mReqQue = RIO.RIOCreateRequestQueue(
		mSock, MAX_RQ_RECV, 1, MAX_RQ_SEND, 1,
		IOManager::GetCQ(mThreadId), IOManager::GetCQ(mThreadId), nullptr);

	if (mReqQue == RIO_INVALID_RQ)
	{
		PrintException(L"Invalid RIO_RQ.");
	}
	
	mDisconnected.store(false);
	OnConnected();

	PostRecv();
}

void Session::Disconnect()
{
	if (!isConnected())
		return;

	mDisconnected.store(true);

	std::lock_guard<std::mutex> lock(mMtx);
	OnDisconnected();
	SocketUtils::CloseSocket(mSock);
}

bool inline Session::isConnected()
{
	return !mDisconnected;
}

void Session::Send(byte* buffer, int length)
{
	SendContext* sendContext = new SendContext();
	sendContext->BufferId = mBufferId;
	sendContext->owner = shared_from_this();

	//memmove(mBuffer + mCircularBuffer.getWritableOffset(), buffer, length);
	//sendContext->Length = length;
	//sendContext->Offset = mCircularBuffer.getWritableOffset();

	PostSend(sendContext);
}

bool Session::PostRecv()
{
	if (!isConnected())
		return false;


	recvBuffer->Clear();
	RecvContext* recvContext = new RecvContext();
	recvContext->owner = shared_from_this();
	recvContext->BufferId = mBufferId;
	recvContext->Length = recvBuffer->GetFreeSize();
	recvContext->Offset = recvBuffer->GetWriteOffset();

	DWORD recvBytes = 0;
	DWORD flag = 0;
	if (!RIO.RIOReceive(mReqQue, (PRIO_BUF)recvContext, 1, flag, recvContext))
	{
		recvContext->owner = nullptr;
		PrintException(L"Faild RIOReceive");
		return false;
	}
	return true;
}

void Session::CompleteRecv(RecvContext* recvContext, DWORD transferred)
{
	recvContext->owner = nullptr;

	if (transferred == 0)
	{
		Disconnect();
		return;
	}

	if (!recvBuffer->OnWrite(transferred))
	{
		Disconnect();
		return;
	}

	auto buf = std::make_shared<byte[]>(transferred);
	std::copy(mBuffer + recvBuffer->GetReadOffset(), mBuffer + recvBuffer->GetDataSize(), buf.get());

	int recvLen = OnRecv(mBuffer, transferred);
	if (recvLen < 0 || recvLen > recvBuffer->GetDataSize())
	{
		Disconnect();
		return;
	}

	if (!recvBuffer->OnRead(recvLen))
	{
		Disconnect();
		return;
	}
	PostRecv();
}

bool Session::PostSend(SendContext* sendContext)
{
	if (!isConnected()) return false;

	if (!RIO.RIOSend(mReqQue, sendContext, 1, NULL, sendContext))
	{
		sendContext->owner = nullptr;
		PrintException(L"Faild RIOSend");
		return false;
	}
	return true;
}


void Session::CompleteSend(SendContext* sendContext, DWORD transferred)
{
	sendContext->owner = nullptr;

	if (transferred == 0)
	{
		Disconnect();
		return;
	}

	OnSend(transferred);
}
