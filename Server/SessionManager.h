#pragma once

#include <mutex>

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
	std::mutex mMtx;
};

extern SessionManager GSessionManager;