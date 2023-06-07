#include "stdafx.h"
#include "IPAddress.hpp"

IPAddress::IPAddress(const wchar_t* ipAddress, u_short port)
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	this->sin_family = AF_INET;
	this->sin_port = htons(port);
	InetPton(AF_INET, ipAddress, &this->sin_addr);
}

std::wstring IPAddress::getAddress()
{
	wchar_t ipstr[16] = L"";
	InetNtop(AF_INET, &this->sin_addr, ipstr, 16);
	return ipstr;
}

u_short IPAddress::getPort()
{
	return ntohs(this->sin_port);
}
