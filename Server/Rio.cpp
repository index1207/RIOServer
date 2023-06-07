#include "stdafx.h"
#include "Rio.hpp"

Rio RIO;

void Rio::Initialize()
{
	GUID functionTableId = WSAID_MULTIPLE_RIO;
	SOCKET dummySock = SocketUtils::CreateSocket(WSA_FLAG_REGISTERED_IO);
	DWORD dwBytes = 0;
	if (::WSAIoctl(dummySock, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID), this, sizeof(RIO_EXTENSION_FUNCTION_TABLE), &dwBytes, nullptr, nullptr))
	{
		throw network_error();
	}
	SocketUtils::CloseSocket(dummySock);
}