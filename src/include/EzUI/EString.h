#pragma once
#include "UIDef.h"
#include <codecvt>
#include <iomanip>

namespace EzUI {
	/// <summary>
	/// utf8字符串
	/// </summary>
	class  UI_EXPORT EString :public std::string {
	public:
		EString();
		virtual ~EString();
		EString(const EString& _right)noexcept;
		EString(EString&& _right)noexcept;
		EString& operator=(const EString& _right)noexcept;
		EString& operator=(EString&& _right)noexcept;
		EString(const std::string& str)noexcept;
		EString(const char* szbuf)noexcept;
		EString(const wchar_t* szbuf)noexcept;
		EString(const std::wstring& wstr)noexcept;
		size_t length() const;//the utf8 Length
		std::wstring unicode() const;
		std::string ansi() const;
		std::vector<std::string> split(const EString& ch_)const;
		void erase(char _ch);
		void erase(size_t pos, size_t count);
		EString replace(char oldChar, char newChar);
		EString replace(const EString& oldText, const EString& newText, bool allReplace = true)const;
		EString toLower()const;
		EString toUpper()const;
		bool operator==(const wchar_t* szbuf)const;
		bool operator==(const std::wstring& wStr)const;
	public:
		//base convert
		static void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr);
		static void UnicodeToAny(const std::wstring& unicode_wstr, UINT codePage, std::string* out_str);
		//
		static void GBKToUTF8(const std::string& str, std::string* outStr);
		static void UTF8ToGBK(const std::string& str, std::string* outStr);
		static void ANSIToUniCode(const std::string& str, std::wstring* outStr);
		static void ANSIToUTF8(const std::string& str, std::string* outStr);
		static void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
		static void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
		static void UTF8ToANSI(const std::string& str, std::string* outStr);
		static void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
		//
		static void Tolower(std::string* str_in_out);
		static void Toupper(std::string* str_in_out);
		static void Erase(std::string* str_in_out, const char& ch);
		static void Replace(std::string* str_in_out, char oldChar, char newChar);
		static void Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll = true);
		static void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);
		//
		static EString ToString(double number, int keepBitSize);
	};
#define utf8(text) EString(L##text)
};