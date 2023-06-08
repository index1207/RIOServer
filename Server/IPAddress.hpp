#pragma once

class IPAddress : private SOCKADDR_IN
{
public:
	IPAddress();
	IPAddress(std::wstring ipAddress, u_short port);
public:
	void setAddress(std::wstring ipAddress);
	void setPort(u_short port);

	std::wstring getAddress();
	u_short getPort();
};

