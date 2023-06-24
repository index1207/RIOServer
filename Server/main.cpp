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

	virtual int OnRecv(byte* buffer, DWORD transffered) override
	{
		std::cout << "[INFO] Received " << std::string(reinterpret_cast<const char*>(buffer), transffered) << '\n';
		//Send(buffer, transffered);
		return transffered;
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

		Listener listener(IPAddress(L"127.0.0.1", 8888), []() { return GSessionManager.RequestSession<EchoSession>(); });
		listener.Start();
	}
	catch (net_exception& e)
	{
		std::wcout << e.what() << '\n';
		getchar();
	}
	return 0;
}