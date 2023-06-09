#pragma once

#include <tchar.h>

class Encoding
{
public:
	template<class DestSTR, class SourceSTR>
	static DestSTR ConvertTo(SourceSTR) = delete;

	template<>
	static std::string ConvertTo<std::string, std::wstring>(std::wstring str);

	template<>
	static std::wstring ConvertTo<std::wstring, std::string>(std::string str);

	template<>
	static std::string ConvertTo<std::string, const wchar_t*>(const wchar_t* str);
};

template<>
inline std::string Encoding::ConvertTo(std::wstring str)
{
	// ANSI Encoding
	auto ansi_cstr = std::make_unique<char[]>(str.length());
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, ansi_cstr.get(), str.length()*2 + 1, 0, 0);
	std::string s = ansi_cstr.release();

	return std::move(s);
}

template<>
inline std::wstring Encoding::ConvertTo(std::string str)
{
	int nLen1 = MultiByteToWideChar(CP_UTF8, 0, &str[0], str.size(), NULL, NULL);
	std::wstring strUni(nLen1, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], str.size(), &strUni[0], nLen1);

	return std::move(strUni);
}

template<>
inline std::string Encoding::ConvertTo(const wchar_t* str)
{
	return ConvertTo<std::string>(std::wstring(str));
}
