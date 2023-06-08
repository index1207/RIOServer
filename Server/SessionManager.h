#pragma once

#include <list>

class Session;

class SessionManager
{
public:
	SessionManager();
	~SessionManager();
public:
	std::shared_ptr<Session> RequestSession();
private:
	int mSessionCount;
};

extern SessionManager GSessionManager;