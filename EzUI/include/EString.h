#pragma once
#include "UIDef.h"
#include <codecvt>

namespace EzUI {
	class  UI_EXPORT EString :public std::string {
	public:
		size_t length() const;//the utf8 length
		EString();
		EString(const std::string& str);
		EString(const char* szbuf);
		EString(const wchar_t* szbuf);
		EString(const std::wstring& wstr);
		std::wstring utf16() const;
		std::string ansi() const;
		EString Erase(const char& _char)const;
		std::vector<std::string> Split(const EString& ch_)const;
		EString Replace(const EString& oldText, const EString& newText)const;
		EString Tolower()const;
		EString Toupper()const;
	public:
		static void ANSIToUniCode(const std::string& str, std::wstring* outStr);
		static void ANSIToUTF8(const std::string& str, std::string* outStr);
		static void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
		static void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
		static void UTF8ToANSI(const std::string& str, std::string* outStr);
		static void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
		static void Tolower(std::string* str_in_out);
		static void Toupper(std::string* str_in_out);
		static void Erase(std::string* str_in_out,const char&ch);
		static void Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText);
		static void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);
	};
};