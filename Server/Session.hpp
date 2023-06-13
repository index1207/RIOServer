#pragma once

#include "RioContext.hpp"

#include <mutex>
#include <atomic>

class Session : public std::enable_shared_from_this<Session>
{
	enum { BUFFER_SIZE = 0x1000 };
public:
	Session(int threadId);
	virtual ~Session();
public:
	void Initialize(SOCKET sock, IPAddress ipAddress);

	void Disconnect();
	bool isConnected();

	void Send(byte* buffer, int length);
public:
	virtual void OnConnected() { };
	virtual void OnDisconnected() { };
	virtual void OnRecv(byte* recvBuffer, DWORD length) { };
	virtual void OnSend(DWORD transferred) { };
public:
	bool PostRecv();
	void CompleteRecv(RecvContext* recvContext, DWORD transferred);

	bool PostSend(SendContext* sendContext);
	void CompleteSend(SendContext* sendContext,DWORD transferred);
protected:
	SOCKET mSock;
	IPAddress mIpAddress;
private:
	std::mutex mMtx;
	std::atomic<bool> mDisconnected;

	int mThreadId;

	RIO_RQ mReqQue;
	RIO_BUFFERID mBufferId;
	PCHAR mBuffer;
};