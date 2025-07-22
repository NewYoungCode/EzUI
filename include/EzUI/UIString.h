#pragma once
#include "UIDef.h"
#include <codecvt>
#include <iomanip>

namespace ezui {
	namespace ui_text {

		//-----------------------------------------------Copy Start-----------------------------------------------
		/// <summary>
		/// utf8字符串
		/// </summary>
		class UI_EXPORT String :public std::string {
		public:
			String();
			virtual ~String();
			String(const String& _right)noexcept;
			String(String&& _right)noexcept;
			String& operator=(const String& _right)noexcept;
			String& operator=(String&& _right)noexcept;
			String(const std::string& str)noexcept;
			String(const char* szbuf)noexcept;
			String(const wchar_t* szbuf)noexcept;
			String(const std::wstring& wstr)noexcept;
			//获取utf8字符串的字符串长度
			virtual size_t length() const final;
			std::wstring unicode() const;
			std::string ansi() const;
			void erase(char _ch);
			void erase(size_t pos, size_t count);
			String replace(char oldChar, char newChar)const;
			String replace(const String& oldText, const String& newText, bool allReplace = true)const;
			String toLower()const;
			String toUpper()const;
			//去除前后空格
			String trim()const;
			//find value count
			size_t count(const String& value)const;
			std::vector<String> split(const String& ch)const;
			bool operator==(const wchar_t* szbuf)const;
			bool operator==(const std::wstring& wStr)const;

			template<typename ...T>
			inline String format(const T &...args) {
				auto bufSize = ::snprintf(nullptr, 0, this->c_str(), std::forward<const T&>(args)...) + 1;  // +1是为了'结束符\0'
				char* buf = new char[bufSize] {0};
				auto count = ::sprintf_s(buf, bufSize, this->c_str(), std::forward<const T&>(args)...);
				String ret(buf);
				delete[] buf;
				return ret;
			}
		};

		//base convert
		UI_EXPORT void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr);
		UI_EXPORT void UnicodeToAny(const std::wstring& unicode_wstr, UINT codePage, std::string* out_str);
		//
		UI_EXPORT void GBKToUTF8(const std::string& str, std::string* outStr);
		UI_EXPORT void UTF8ToGBK(const std::string& str, std::string* outStr);
		UI_EXPORT void ANSIToUniCode(const std::string& str, std::wstring* outStr);
		UI_EXPORT void ANSIToUTF8(const std::string& str, std::string* outStr);
		UI_EXPORT void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
		UI_EXPORT void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
		UI_EXPORT void UTF8ToANSI(const std::string& str, std::string* outStr);
		UI_EXPORT void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
		//
		UI_EXPORT void Tolower(std::string* str_in_out);
		UI_EXPORT void Toupper(std::string* str_in_out);
		UI_EXPORT void Erase(std::string* str_in_out, char ch);
		UI_EXPORT void Replace(std::string* str_in_out, char oldChar, char newChar);
		UI_EXPORT size_t Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll = true);
		UI_EXPORT void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);
		//
		UI_EXPORT String ToString(double number, size_t keepBitSize);
		//-----------------------------------------------Copy End-----------------------------------------------
	};
	using UIString = ui_text::String;
};