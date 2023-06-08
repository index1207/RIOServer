#include "stdafx.h"
#include "SocketUtils.hpp"

#include "Exception.hpp"

void SocketUtils::Initialize()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		throw network_error();
	}
}

void SocketUtils::Terminate()
{
	WSACleanup();
}

SOCKET SocketUtils::CreateSocket(DWORD flag)
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, flag);
}

void SocketUtils::CloseSocket(SOCKET sock)
{
	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
}