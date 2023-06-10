#pragma once

#include <functional>

#include "Session.hpp"

class Listener
{
	friend IPAddress;
public:
	Listener(IPAddress ipAddress);
	virtual ~Listener();
public:
	void Start(std::function<std::shared_ptr<Session>()> sessionFactory);
private:
	SOCKET mListenSock;
	IPAddress mIpAddress;
};