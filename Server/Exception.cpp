#include "stdafx.h"
#include "Exception.hpp"



net_exception::net_exception(std::wstring info) : logstr(info)
{
}

net_exception::net_exception(std::string info)
{
    logstr = Encoding::ConvertTo<std::wstring>(info);
}

char const* net_exception::what()
{
    auto excpt = Encoding::ConvertTo<std::string>(logstr + getErrorMessage());
    return excpt.c_str();
}

std::wstring net_exception::getErrorMessage()
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

    std::wstring t = res.release()->c_str();
    return t;
}

void printException(std::string loc, std::wstring expr)
{
    std::wcout << std::format(L"[ERROR] {} {}\n", Encoding::ConvertTo<std::wstring>(loc), expr);
}