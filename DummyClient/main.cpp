#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSAStartup(MAKEWORD(2, 2), new WSADATA);

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10001);
	inet_pton(AF_INET, "10.53.68.136", &addr.sin_addr);

	if (SOCKET_ERROR == connect(s, (SOCKADDR*)&addr, sizeof(addr)))
		return 0;
	else std::cout << "connected!\n";

	std::string str;
	std::cin >> str;

	while (true)
	{
		int r = send(s, str.c_str(), str.size(), NULL);
		if (r > 0) return 0;
		Sleep(500);
	}

	WSACleanup();
	return 0;
}