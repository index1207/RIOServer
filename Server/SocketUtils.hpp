#pragma once

class SocketUtils
{
public:
	static void Initialize();
	static void Terminate();

	static SOCKET CreateSocket(DWORD flag);
	static void CloseSocket(SOCKET& sock);

	template<typename T>
	static void setsockopt(SOCKET sock, int level, int optname, T value);
};

template<typename T>
inline void SocketUtils::setsockopt(SOCKET sock, int level, int optname, T value)
{
	::setsockopt(sock, level, optname, reinterpret_cast<const char*>(&value), sizeof(T));
}