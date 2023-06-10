#include <iostream>
#include <vector>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return -1;
	}

	auto s = ::socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) return -1;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	inet_pton(AF_INET, "192.168.0.67", &addr.sin_addr);

	if (SOCKET_ERROR == ::connect(s, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)))
	{
		return -1;
	}

	while (true)
	{
		std::string str;
		std::getline(std::cin, str);
		send(s, &str[0], str.size(), 0);
		std::vector<char> buf(str.length(), 0);
		recv(s, buf.data(), buf.size(), 0);
		std::cout << "recv: " << std::string{ buf.begin(), buf.end() }.c_str() << '\n';
	}

	::WSACleanup();
}