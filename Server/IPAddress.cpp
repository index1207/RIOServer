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

IPAddress::IPAddress(const SOCKADDR_IN& saddr)
{
	memcpy(this, &saddr, sizeof(SOCKADDR_IN));
}

IPAddress IPAddress::operator=(const SOCKADDR_IN& saddr)
{
	IPAddress ipAddress;
	memcpy(&ipAddress, &saddr, sizeof(SOCKADDR_IN));
	return std::move(ipAddress);
}

void IPAddress::SetAddress(std::wstring ipAddress)
{
	InetPton(AF_INET, ipAddress.c_str(), &this->sin_addr);
}

void IPAddress::SetPort(u_short port)
{
	this->sin_port = htons(port);
}

std::wstring IPAddress::GetAddress()
{
	wchar_t ipstr[16] = L"";
	InetNtop(AF_INET, &this->sin_addr, ipstr, 16);
	return ipstr;
}

u_short IPAddress::GetPort()
{
	return ntohs(this->sin_port);
}
