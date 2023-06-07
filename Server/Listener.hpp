#pragma once

class Listener
{
	friend IPAddress;
public:
	Listener(IPAddress ipAddress);
	virtual ~Listener();
public:
	void Start();
private:
	SOCKET mListenSock;
	IPAddress mIpAddress;
};
