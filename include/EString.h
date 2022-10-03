#pragma once
#include "UIDef.h"
#include <codecvt>

namespace EzUI {
	class  UI_EXPORT EString :public std::string {
	public:
		enum
		{
			ANSI = CP_ACP,
			UTF8 = CP_UTF8
		};
	public:
		size_t length() const;
		EString();
		EString(const std::string& str, int codePage = EString::UTF8);
		EString(const char* szbuf, int codePage = EString::UTF8);
		EString(const wchar_t* szbuf);
		EString(const std::wstring& wstr);
		std::wstring utf16() const;
		EString Erase(CHAR _char)const;
		std::vector<EString> Split(const EString& ch_)const;
		EString Replace(const EString& oldText, const EString& newText)const;
		EString Tolower()const;
		EString Toupper()const;
	public:
		static void ANSIToUniCode(const std::string& str, std::wstring* outStr);
		static void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
		static size_t Replace(EString& str, const EString& oldText, const EString& newText);
	};
#define utf8(szbuf)  EString(szbuf,EString::ANSI)
};