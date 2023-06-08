#pragma once

#include <mutex>
#include <atomic>

class Session
{
public:
	Session(int threadId);
public:
	void Initialize(SOCKET sock, IPAddress ipAddress);
private:
	SOCKET mSock;
	IPAddress mIpAddress;
	
	std::mutex mMtx;
	std::atomic<bool> mDisconnected;

	int mThreadId;
	RIO_RQ mReqQue;
};