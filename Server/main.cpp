#include "stdafx.h"

#include "Engine.hpp"
#include "SessionManager.h"

class EchoSession : public Session
{
public:
	EchoSession(int threadId) : Session(threadId)
	{
	}
	~EchoSession() {}

	virtual void OnConnected() override
	{
		std::wcout << std::format(L"[INFO] Client Connected {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());
	}
	
	virtual void OnDisconnected() override
	{
		std::wcout << std::format(L"[INFO] Client Disconnected {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());
	}

	virtual void OnRecv(DWORD transffered) override
	{
		std::wcout << L"[INFO] Received " << transffered << " Bytes\n";
		Send((BYTE*)"", transffered);
	}
	virtual void OnSend(DWORD transffered) override
	{
		std::wcout << L"[INFO] Send " << transffered << " Bytes\n";
	}
};

int main()
{
	try
	{
		IOManager ioManager;
		ioManager.Start();

		Listener listener(IPAddress(L"192.168.0.67", 8888));
		listener.Start([]() {
			return GSessionManager.RequestSession<EchoSession>();
		});
	}
	catch (net_exception& e)
	{
		std::wcout << e.what() << '\n';
		getchar();
	}
	return 0;
}