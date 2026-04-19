#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace Text {
	//-----------------------------------------------Copy Start-----------------------------------------------
		/// <summary>
		/// utf8字符串
		/// </summary>
	class  String :public std::string {
	public:
		String();
		virtual ~String();
		String(const String& _right)noexcept;
		String(String&& _right)noexcept;
		String& operator=(const String& _right)noexcept;
		String& operator=(String&& _right)noexcept;
		String(const std::string& str)noexcept;
		String(const char* szbuf)noexcept;
		String(const char* pStr, size_t count)noexcept;
		String(const wchar_t* szbuf)noexcept;
		String(const std::wstring& wstr)noexcept;
		//返回utf8字符个数
		size_t utf8Length() const;
		//获取unicode字符串
		std::wstring unicode() const;
		//转为当前系统可以正常显示的字符串
		std::string ansi() const;
		void erase(char _ch);
		void erase(size_t pos, size_t count);
		String replace(char oldChar, char newChar)const;
		String replace(const String& oldText, const String& newText, bool allReplace = true)const;
		String toLower()const;
		String toUpper()const;
		int toInt()const;
		float toFloat()const;
		double toDouble()const;
		int64_t toInt64()const;
		//去除前后空格
		String trim()const;
		//find value count
		size_t count(const String& value)const;
		//字符串分割
		std::vector<String> split(const String& ch)const;
		//字符串分割
		std::vector<String> split(char ch)const;
		bool operator==(const wchar_t* szbuf)const;
		bool operator==(const std::wstring& wStr)const;
		bool operator!=(const wchar_t* szbuf)const;
		bool operator!=(const std::wstring& wStr)const;
	private:
		// 默认模板:非字符串类型直接返回
		template<typename T>
		inline T convert_arg(const T& val) { return val; }
		// 字符串类型转换为 const char*
		inline const char* convert_arg(const String& s) { return s.c_str(); }
		inline const char* convert_arg(const std::string& s) { return s.c_str(); }
		inline const char* convert_arg(const char* s) { return s; }
	public:
		//格式化自身字符串并返回
		template<typename ...T>
		inline String& args(const T &...args) {
			auto bufSize = ::snprintf(NULL, 0, this->c_str(), convert_arg(args)...) + 1;  // +1是为了'结束符\0'
			char* buf = new char[bufSize] {0};
			auto count = ::sprintf_s(buf, bufSize, this->c_str(), convert_arg(args)...);
			String ret(buf);
			*this = ret;
			delete[] buf;
			return *this;
		}
		//从当前系统的字符串转为UIString
		static String FromLocal(const std::string& localStr);
		/// <summary>
		/// 数字转为字符串
		/// </summary>
		/// <param name="number">数字</param>
		/// <param name="keepBitSize">保留小数格式</param>
		/// <returns></returns>
		static String ToString(double number, size_t keepBitSize);
	};

	//base convert
	void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr);
	void UnicodeToAny(const std::wstring& unicode_wstr, UINT codePage, std::string* out_str);
	//
	void GBKToUTF8(const std::string& str, std::string* outStr);
	void UTF8ToGBK(const std::string& str, std::string* outStr);
	void ANSIToUniCode(const std::string& str, std::wstring* outStr);
	void ANSIToUTF8(const std::string& str, std::string* outStr);
	void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
	void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
	void UTF8ToANSI(const std::string& str, std::string* outStr);
	void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
	//
	void Tolower(std::string* str_in_out);
	void Toupper(std::string* str_in_out);
	void Erase(std::string* str_in_out, char ch);
	void Replace(std::string* str_in_out, char oldChar, char newChar);
	size_t Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll = true);
	void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);
	//-----------------------------------------------Copy End-----------------------------------------------
};
using u8String = Text::String;
