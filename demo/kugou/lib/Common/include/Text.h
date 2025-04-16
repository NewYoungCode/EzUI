#pragma once
#include "General.h"
namespace Text {
	DWORD IPStringToDWORD(const std::string&ipStr);
	std::string IPDWORDToString(DWORD&ip);
	//寻找字符串里面字符个数
	size_t FindCount(const std::string&str, const std::string&ch_);
	//去除前后空格
	void Trim(std::string &str);
	//包含大写的字母转小写
	std::string ToLower(const std::string&str);
	//包含小写的字母转大写
	std::string ToUpper(const std::string&str);
	//ANSIToUniCode
	std::wstring ANSIToUniCode(const std::string &wstr);
	//UnicodeToANSI
	std::string UnicodeToANSI(const std::wstring &wstr);
	//UnicodeToUTF8
	std::string UnicodeToUTF8(const std::wstring &wstr);
	//ANSIToUTF8
	std::string ANSIToUTF8(const std::string &str);
	//UTF8ToANSI
	std::string UTF8ToANSI(const std::string &str);
	//截取字符串(宽字符)
	std::wstring Substr(const std::wstring &str, size_t starIndex, size_t count = std::string::npos);
	//截取字符串(窄字符)
	std::string Substr(const std::string &str, size_t starIndex, size_t count = std::string::npos);
	//直接替换字符并且返回修改个数
	size_t Replace(std::string &str, const std::string &oldText, const std::string & newText);
	//直接替换字符并且返回修改之后的字符串
	std::string ReplaceAll(const std::string &str, const std::string & oldText, const std::string & newText);
	//字符串分割
	std::vector<std::string> Split(const std::string& str, const std::string& ch);
	//删除字符串
	bool EraseString(std::string &out_in_str, const std::string& in_oldStr);
	template<typename ...T>
	inline void Format(LPTSTR buf, size_t strCount, LPCTSTR formatStr, T...args) {
#ifdef UNICODE
		swprintf_s((buf), strCount, formatStr, std::forward<T>(args)...);
#else
		sprintf_s((buf), strCount, formatStr, std::forward<T>(args)...);
#endif
	}

#define AUTOTEXT(str) const_cast<LPTSTR>(Text::Auto(str).c_str())
	inline auto Auto(const std::string&str) {
#ifdef UNICODE
		return ANSIToUniCode(str);
#else
		return str;
#endif 
	}

	inline	auto Auto(const std::wstring&wstr) {
#ifdef UNICODE
		return wstr;
#else
		return UnicodeToANSI(wstr);
#endif 
	}
}


