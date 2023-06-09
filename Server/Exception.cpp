#include "stdafx.h"
#include "Exception.hpp"

#include "Encoding.hpp"

network_error::network_error(std::string info) : logstr(info)
{
}

char const* network_error::what()
{
    return (logstr + getErrorMessage()).c_str();
}

void network_error::PrintExcept()
{
    std::cout << logstr + getErrorMessage() << '\n';
}

std::string network_error::getErrorMessage()
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);

    auto res = std::make_unique<std::string>(Encoding::ConvertTo<std::string>(reinterpret_cast<const wchar_t*>(lpMsgBuf)));
    LocalFree(lpMsgBuf);

    return res.release()->c_str();
}
