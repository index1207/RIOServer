#pragma once

#include <mutex>

class Session;

class SessionManager
{
public:
	SessionManager();
	~SessionManager();
public:
	template<class SessionType>
	std::shared_ptr<SessionType> RequestSession();
private:
	std::atomic<int> mSessionCount;
};

template<class SessionType>
inline std::shared_ptr<SessionType> SessionManager::RequestSession()
{
	if (mSessionCount == INT32_MAX)
		mSessionCount = 0;

	return std::make_shared<SessionType>(mSessionCount++ % MAX_THREAD);
}

extern SessionManager GSessionManager;