#include "stdafx.h"
#include "SocketUtils.hpp"

#include "Exception.hpp"

void SocketUtils::Initialize()
{
	auto wsaData = std::make_shared<WSADATA>();
	if (::WSAStartup(MAKEWORD(2, 2), wsaData.get()))
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

void SocketUtils::CloseSocket(SOCKET& sock)
{
	closesocket(sock);
	sock = INVALID_SOCKET;
}