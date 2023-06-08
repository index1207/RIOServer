#pragma once

class network_error : public std::runtime_error
{
public:
	network_error();
	virtual const wchar_t* what();
};
