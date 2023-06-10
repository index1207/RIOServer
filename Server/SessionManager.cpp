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
