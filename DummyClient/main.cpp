#include <iostream>
#include <vector>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <future>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	if (argc <= 1) return -1;

	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return -1;
	}

	auto s = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (s == INVALID_SOCKET) return -1;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(std::stoi(argv[1]));
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	if (SOCKET_ERROR == ::connect(s, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)))
	{
		return -1;
	}

	std::cout << "connected!\n";

	std::string data = "HELLO WORLD!";
	while (true)
	{
		send(s, data.c_str(), data.size(), 0);
	}

	::WSACleanup();
}