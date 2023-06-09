#include "stdafx.h"
#include "Session.hpp"

#include "Rio.hpp"
#include "IOManager.hpp"

#include <format>


Session::Session(int threadId)
	: mThreadId(threadId), mSock(INVALID_SOCKET), mReqQue(RIO_INVALID_RQ), mDisconnected(true)
{
	mBuffer = reinterpret_cast<PCHAR>(::VirtualAllocEx(GetCurrentProcess(), 0, BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	if (mBuffer == nullptr)
	{
		throw std::runtime_error("Virtual memory allocation error.");
	}

	mBufferId = RIO.RIORegisterBuffer(mBuffer, BUFFER_SIZE);
	if (mBufferId == RIO_INVALID_BUFFERID)
	{
		THROW_NET_EXCEPTION;
	}
}

Session::~Session()
{
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
		THROW_NET_EXCEPTION;
	}
	
	mDisconnected.store(false);

	std::wcout << std::format(L"Client Connected {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());

	PostRecv();
}

void Session::Disconnect()
{
	std::lock_guard<std::mutex> lock(mMtx);
	if (!isConnected())
		return;

	std::wcout << std::format(L"Client Disconnected {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());
	mDisconnected.store(true);

	SocketUtils::CloseSocket(mSock);
}

bool Session::isConnected()
{
	return !mDisconnected;
}

bool Session::PostRecv()
{
	if (!isConnected())
		return false;

	RecvContext* recvContext = new RecvContext();
	recvContext->session = shared_from_this();
	recvContext->BufferId = mBufferId;
	recvContext->Length = BUFFER_SIZE;
	recvContext->Offset = 0;

	DWORD recvBytes = 0;
	DWORD flag = 0;
	if (!RIO.RIOReceive(mReqQue, (PRIO_BUF)recvContext, 1, flag, recvContext))
	{
		recvContext->session = nullptr;
		PRINT_NET_EXCEPTION;
		return false;
	}
	return true;
}

void Session::CompleteRecv(RecvContext* recvContext, DWORD transferred)
{
	recvContext->session = nullptr;

	if (transferred == 0)
	{
		Disconnect();
		return;
	}
	
	PostRecv();
}

void Session::PostSend()
{
}

void Session::CompleteSend(SendContext* sendContext, DWORD transferred)
{
	sendContext->session = nullptr;

	if (transferred == 0)
	{
		Disconnect();
		return;
	}
}
