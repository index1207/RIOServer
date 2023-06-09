#pragma once

#include "Encoding.hpp"

#include <format>

#define log ("[" + std::string(strrchr(__FILE__, '\\')+1) + ":" + std::to_string(__LINE__) + "] " + std::string(__FUNCTION__) + "() ")
#define PRINT_NET_EXCEPTION network_error{Encoding::ConvertTo<std::wstring>(log)}.PrintExcept()
#define PRINT_EXCEPTION(text) network_error{Encoding::ConvertTo<std::wstring>(log)}.PrintExcept(TEXT(text))
#define THROW_NET_EXCEPTION throw network_error(Encoding::ConvertTo<std::wstring>(log))
#define THROW_ASSERT(expr)		\
{								\
	if(!(expr))	{				\
		THROW_NET_EXCEPTION;	\
		__analysis_assume(expr);\
	}							\
}

class network_error : public std::exception
{
public:
	network_error() = default;
	network_error(std::wstring info);
	virtual char const* what();
public:
	void PrintExcept(std::wstring message = L"");
private:
	static std::wstring getErrorMessage();
private:
	std::wstring logstr;
};
