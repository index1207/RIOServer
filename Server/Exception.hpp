#pragma once

#include <format>

#define log ("[" + std::string(strrchr(__FILE__, '\\')+1) + ":" + std::to_string(__LINE__) + "] " + std::string(__FUNCTION__) + "() ")
#define PRINT_NET_EXCEPTION network_error{log}.PrintExcept()
#define THROW_NET_EXCEPTION throw network_error(log)
#define THROW_ASSERT(expr)		\
{								\
	if(!(expr))	{				\
		THROW_NET_EXCEPTION;				    \
		__analysis_assume(expr);\
	}							\
}

class network_error : public std::exception
{
public:
	network_error() = default;
	network_error(std::string info);
	virtual char const* what();
public:
	void PrintExcept();
private:
	static std::string getErrorMessage();
private:
	std::string logstr;
};
