#include "stdafx.h"

#include "Engine.hpp"

int main()
{
	try
	{
		IOManager ioManager;
		ioManager.Start();

		Listener listener(IPAddress(L"127.0.0.1", 8888));
		listener.Start();
	}
	catch (net_exception& e)
	{
		std::wcout << e.what() << '\n';
		getchar();
	}
	return 0;
}