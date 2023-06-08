#include "stdafx.h"
#include "Session.hpp"

#include "Rio.hpp"
#include "IOManager.hpp"

Session::Session(SOCKET sock, IPAddress ipAddress)
{
	mSock = sock;
	mIpAddress = ipAddress;

	SocketUtils::setsockopt(mSock, IPPROTO_TCP, TCP_NODELAY, true);
	//mReqQue = RIO.RIOCreateRequestQueue(mSock, MAX_RQ_RECV, 1, MAX_RQ_SEND, 1, IOManager::GetCQ(), );
}

void Session::Initialize(RIO_CQ cq)
{
	mReqQue = RIO.RIOCreateRequestQueue(mSock, MAX_RQ_RECV, 1, MAX_RQ_SEND, 1, cq, cq, NULL);
}
