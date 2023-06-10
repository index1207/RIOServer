#pragma once

#include "Encoding.hpp"

#include <format>

#define log ("[" + std::string(strrchr(__FILE__, '\\')+1) + ":" + std::to_string(__LINE__) + "] " + std::string(__FUNCTION__) + "() ")
#define CRASH(excpType) throw excpType(log)
#define ASSERT_CRASH(excpType, expr) \
{									 \
	if(!(expr))	{					 \
		CRASH(excpType);			 \
		__analysis_assume(expr);	 \
	}								 \
}
#define PrintException(expr) do { printException(log, expr); } while(false)

void printException(std::string loc, std::wstring expr);

class net_exception : public std::exception
{
public:
	net_exception() = default;
	net_exception(std::wstring info);
	net_exception(std::string info);
	virtual char const* what();
private:
	static std::wstring getErrorMessage();
private:
	std::wstring logstr;
};
