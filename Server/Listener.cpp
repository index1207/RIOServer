#include "stdafx.h"
#include "Listener.hpp"

#include "IPAddress.hpp"

#include <format>
 
Listener::Listener(IPAddress ipAddress) : mIpAddress(ipAddress)
{
	mListenSock = SocketUtils::CreateSocket(WSA_FLAG_REGISTERED_IO);
	if (mListenSock == INVALID_SOCKET)
	{
		CRASH(net_exception);
	}

	SocketUtils::setsockopt(mListenSock, SOL_SOCKET, SO_REUSEADDR, true);
	
	if (SOCKET_ERROR == ::bind(mListenSock, reinterpret_cast<SOCKADDR*>(&mIpAddress), sizeof(SOCKADDR_IN)))
	{
		CRASH(net_exception);
	}
}

 
Listener::~Listener()
{
	SocketUtils::CloseSocket(mListenSock);
}

 
void Listener::Start(std::function<std::shared_ptr<Session>()> sessionFactory)
{
	if (SOCKET_ERROR == ::listen(mListenSock, SOMAXCONN))
	{
		CRASH(net_exception);
	}

	std::wcout << std::format(L"Server Running on {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());

	while (true)
	{
		SOCKET clientSock = ::accept(mListenSock, nullptr, nullptr);
		if (clientSock == INVALID_SOCKET)
		{
			PrintException(TEXT("Invaild Socket"));
			continue;
		}
		SOCKADDR_IN clientAddr;
		int addrsize = sizeof(SOCKADDR_IN);
		getpeername(clientSock, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrsize);

		auto session = sessionFactory();
		session->Initialize(clientSock, clientAddr);
	}
}
