#include "stdafx.h"
#include "Exception.hpp"



network_error::network_error(std::wstring info) : logstr(info)
{
}

char const* network_error::what()
{
    return Encoding::ConvertTo<std::string>(logstr + getErrorMessage()).c_str();
}

void network_error::PrintExcept(std::wstring message)
{
    if (message == L"")
        std::wcout << logstr + getErrorMessage();
    else
        std::wcout << logstr + message << '\n';
}

std::wstring network_error::getErrorMessage()
{
    const int errorCode = WSAGetLastError();
    if (errorCode == 0) return L"잘못된 작업입니다.\n";
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);

    auto res = std::make_unique<std::wstring>(reinterpret_cast<const wchar_t*>(lpMsgBuf));
    LocalFree(lpMsgBuf);

    return res.release()->c_str();
}
