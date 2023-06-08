#include "stdafx.h"
#include "SessionManager.h"

#include "Session.hpp"
#include "IOManager.hpp"

SessionManager GSessionManager;

SessionManager::SessionManager() : mSessionCount(0)
{
}

SessionManager::~SessionManager()
{
}

std::shared_ptr<Session> SessionManager::RequestSession()
{
	int threadId = mSessionCount % MAX_THREAD;
	auto session = std::make_shared<Session>(threadId);
	
	return session;
}
