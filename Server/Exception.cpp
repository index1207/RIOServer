#include "stdafx.h"
#include "Exception.hpp"

network_error::network_error() : std::runtime_error("Network Error")
{
}

const char* network_error::what()
{
	const int errorCode = ::WSAGetLastError();
	const char* s = nullptr;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&s, 0, NULL);
	char result[512] = "";
	strcpy_s(result, s);
	LocalFree(reinterpret_cast<HANDLE>(&s));
	return result;
}