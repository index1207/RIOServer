#pragma once

class IPAddress : private SOCKADDR_IN
{
public:
	IPAddress();
	IPAddress(std::wstring ipAddress, u_short port);

	IPAddress(const SOCKADDR_IN& saddr);
public:
	IPAddress operator=(const SOCKADDR_IN& saddr);
public:
	void SetAddress(std::wstring ipAddress);
	void SetPort(u_short port);

	std::wstring GetAddress();
	u_short GetPort();
};

