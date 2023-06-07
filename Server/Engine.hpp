#pragma once
#include "RIO.hpp"
#include "IOManager.hpp"
#include "Listener.hpp"

enum Config
{
	MAX_THREAD = 4,
	MAX_RQ_SEND = 32,
	MAX_RQ_RECV = 32,
	MAX_CLIENT_PER_THREAD = 1024,
	MAX_CQ_SIZE = (MAX_RQ_SEND + MAX_RQ_RECV) * MAX_CLIENT_PER_THREAD,
	MAX_RIORESULT = 256
};

class Engine
{
public:
	Engine();
	~Engine();
};

extern Engine GEngine;
