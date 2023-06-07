#pragma once

class IPAddress : private SOCKADDR_IN
{
public:
	IPAddress() = default;
	IPAddress(const wchar_t* ipAddress, u_short port);
public:
	std::wstring getAddress();
	u_short getPort();
};

