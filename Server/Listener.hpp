#pragma once

#include <functional>

#include "Session.hpp"

class Listener
{
	friend IPAddress;
	using SessionFactoryType = std::function<std::shared_ptr<Session>()>;
public:
	Listener(IPAddress ipAddress, SessionFactoryType sessionFactory);
	virtual ~Listener();
public:
	void Start();
private:
	SOCKET mListenSock;
	IPAddress mIpAddress;
	SessionFactoryType mSessionFactory;
};