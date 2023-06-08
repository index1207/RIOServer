#include "stdafx.h"
#include "Listener.hpp"

#include "IPAddress.hpp"

#include <format>

Listener::Listener(IPAddress ipAddress) : mIpAddress(ipAddress)
{
	mListenSock = SocketUtils::CreateSocket(WSA_FLAG_REGISTERED_IO);
	if (mListenSock == INVALID_SOCKET)
	{
		throw network_error();
	}

	SocketUtils::setsockopt(mListenSock, SOL_SOCKET, SO_REUSEADDR, true);
	
	if (SOCKET_ERROR == ::bind(mListenSock, reinterpret_cast<SOCKADDR*>(&mIpAddress), sizeof(SOCKADDR_IN)))
	{
		throw network_error();
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
		throw network_error();
	}

	std::wcout << std::format(L"Server Running on {}:{}\n", mIpAddress.getAddress(), mIpAddress.getPort());

	while (true)
	{
		SOCKET clientSock = ::accept(mListenSock, nullptr, nullptr);
		if (clientSock == INVALID_SOCKET)
		{
			throw network_error();
		}
		SOCKADDR_IN clientAddr;
		int addrsize = sizeof(SOCKADDR_IN);
		getpeername(clientSock, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrsize);

		/* TODO: */

		std::cout << "Connected client\n";
	}
}
