#include "stdafx.h"
#include "Listener.hpp"

#include "IPAddress.hpp"
#include "SessionManager.h"
#include "Session.hpp"

#include <format>


Listener::Listener(IPAddress ipAddress) : mIpAddress(ipAddress)
{
	mListenSock = SocketUtils::CreateSocket(WSA_FLAG_REGISTERED_IO);
	if (mListenSock == INVALID_SOCKET)
	{
		THROW_NET_EXCEPTION;
	}

	SocketUtils::setsockopt(mListenSock, SOL_SOCKET, SO_REUSEADDR, true);
	
	if (SOCKET_ERROR == ::bind(mListenSock, reinterpret_cast<SOCKADDR*>(&mIpAddress), sizeof(SOCKADDR_IN)))
	{
		THROW_NET_EXCEPTION;
	}
}

Listener::~Listener()
{
	SocketUtils::CloseSocket(mListenSock);
}

void Listener::Start()
{
	if (SOCKET_ERROR == ::listen(mListenSock, SOMAXCONN))
	{
		THROW_NET_EXCEPTION;
	}

	std::wcout << std::format(L"Server Running on {}:{}\n", mIpAddress.GetAddress(), mIpAddress.GetPort());

	while (true)
	{
		SOCKET clientSock = ::accept(mListenSock, nullptr, nullptr);
		if (clientSock == INVALID_SOCKET)
		{
			PRINT_EXCEPTION("aa");
			continue;
		}
		SOCKADDR_IN clientAddr;
		int addrsize = sizeof(SOCKADDR_IN);
		getpeername(clientSock, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrsize);

		auto session = GSessionManager.RequestSession();
		session->Initialize(clientSock, clientAddr);
	}
}
