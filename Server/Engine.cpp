#include "stdafx.h"
#include "Engine.hpp"
#include "Rio.hpp"

Engine GEngine;

Engine::Engine()
{
	SocketUtils::Initialize();
	RIO.Initialize();
}

Engine::~Engine()
{
	SocketUtils::Terminate();
}
