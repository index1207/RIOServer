#pragma once

class Session
{
public:
	Session(SOCKET sock, IPAddress ipAddress);
public:
	void Initialize(RIO_CQ cq);
private:
	SOCKET mSock;
	IPAddress mIpAddress;

	RIO_RQ mReqQue;
};