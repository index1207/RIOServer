#include "stdafx.h"
#include "Engine.hpp"
#include "IOManager.hpp"

int main()
{
	IOManager ioManager;
	ioManager.Start();

	Listener listener(IPAddress(L"127.0.0.1", 8888));
	listener.Start();
}