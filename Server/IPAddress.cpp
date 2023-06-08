#include "stdafx.h"
#include "IPAddress.hpp"

IPAddress::IPAddress()
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	this->sin_family = AF_INET;
}

IPAddress::IPAddress(std::wstring ipAddress, u_short port)
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	this->sin_family = AF_INET;
	this->sin_port = htons(port);
	InetPton(AF_INET, ipAddress.c_str(), &this->sin_addr);
}

void IPAddress::setAddress(std::wstring ipAddress)
{
	InetPton(AF_INET, ipAddress.c_str(), &this->sin_addr);
}

void IPAddress::setPort(u_short port)
{
	this->sin_port = htons(port);
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
