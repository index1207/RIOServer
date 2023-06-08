#include "stdafx.h"
#include "Engine.hpp"
#include "IOManager.hpp"

int main()
{
	try
	{
		IOManager ioManager;
		ioManager.Start();

		Listener listener(IPAddress(L"127.0.0.1", 8888));
		listener.Start();
	}
	catch (network_error& e)
	{
		std::wcout << e.what() << '\n';
	}
}