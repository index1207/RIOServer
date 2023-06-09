#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <atomic>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#include "Exception.hpp"
#include "SocketUtils.hpp"
#include "IPAddress.hpp"

#pragma comment(lib, "ws2_32.lib")

enum Config
{
	MAX_THREAD = 8,
	MAX_CLIENT = 1024,
	MAX_RQ_SEND = 32,
	MAX_RQ_RECV = 32,
	MAX_CLIENT_PER_THREAD = MAX_CLIENT / MAX_THREAD,
	MAX_CQ_SIZE = (MAX_RQ_SEND + MAX_RQ_RECV) * MAX_CLIENT_PER_THREAD,
	MAX_RIORESULT = 256
};
