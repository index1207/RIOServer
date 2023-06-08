#include "stdafx.h"
#include "Exception.hpp"

network_error::network_error() : std::runtime_error("Network Error")
{
}

const wchar_t* network_error::what()
{
	const int errorCode = ::WSAGetLastError();
	const wchar_t* s = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	wchar_t result[127] = L"";
	lstrcpyW(result, s);
	LocalFree(reinterpret_cast<HANDLE>(&s));
	return result;
}