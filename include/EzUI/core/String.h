#pragma once
#include "UIDef.h"

namespace ezui {
	namespace detail {
		//-----------------------------------------------Copy Start-----------------------------------------------
		/// UTF-8 字符串类，继承自 std::string。
		/// 内部存储为 UTF-8 编码，建议使用宽字符构造（std::wstring 或 wchar_t*）
		/// 以保证编码正确。
		/// 注意：不要通过 std::string* 删除动态分配的 String，因为 std::string 没有虚析构函数。
		class String :public std::string {
		public:
			EZUI_API String()EZUI_NOEXCEPT;
			EZUI_API virtual ~String();

			EZUI_API String(const String& right)EZUI_NOEXCEPT;
			EZUI_API String(String&& right)EZUI_NOEXCEPT;
			EZUI_API String& operator=(const String& right)EZUI_NOEXCEPT;
			EZUI_API String& operator=(String&& right)EZUI_NOEXCEPT;

			EZUI_API String(const char* szbuf)EZUI_NOEXCEPT;
			EZUI_API String(const char* pStr, size_t count)EZUI_NOEXCEPT;
			EZUI_API String(const std::string& str)EZUI_NOEXCEPT;

			EZUI_API String(const wchar_t* szbuf)EZUI_NOEXCEPT;
			EZUI_API String(const std::wstring& wstr)EZUI_NOEXCEPT;

			//返回utf8字符个数
			EZUI_API size_t utf8Length() const;
			//获取unicode字符串
			EZUI_API std::wstring unicode() const;
			//转为当前系统可以正常显示的字符串
			EZUI_API std::string ansi() const;
			EZUI_API void erase(char _ch);
			EZUI_API void erase(size_t pos, size_t count);
			EZUI_API String replace(char oldChar, char newChar)const;
			EZUI_API String replace(const String& oldText, const String& newText, bool allReplace = true)const;
			EZUI_API String toLower()const;
			EZUI_API String toUpper()const;
			EZUI_API int toInt()const;
			EZUI_API float toFloat()const;
			EZUI_API double toDouble()const;
			EZUI_API int64_t toInt64()const;
			//判断字符串是否包含一个字符串
			EZUI_API bool contains(const String& str)const EZUI_NOEXCEPT;
			//去除前后空格
			EZUI_API String trim()const;
			//find value count
			EZUI_API size_t count(const String& value)const;
			//字符串分割
			EZUI_API std::vector<String> split(const String& ch)const;
			//字符串分割
			EZUI_API std::vector<String> split(char ch)const;
			EZUI_API bool operator==(const wchar_t* szbuf)const;
			EZUI_API bool operator==(const std::wstring& wStr)const;
			EZUI_API bool operator!=(const wchar_t* szbuf)const;
			EZUI_API bool operator!=(const std::wstring& wStr)const;
		public:
			//从当前系统的字符串转为utf8字符串
			EZUI_API static String fromLocal(const std::string& localStr);
		};

		//base convert
		EZUI_API void AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr);
		EZUI_API void UnicodeToAny(const std::wstring& unicode_wstr, UINT codePage, std::string* out_str);
		//
		EZUI_API void GBKToUTF8(const std::string& str, std::string* outStr);
		EZUI_API void UTF8ToGBK(const std::string& str, std::string* outStr);
		EZUI_API void ANSIToUniCode(const std::string& str, std::wstring* outStr);
		EZUI_API void ANSIToUTF8(const std::string& str, std::string* outStr);
		EZUI_API void UnicodeToANSI(const std::wstring& wstr, std::string* outStr);
		EZUI_API void UnicodeToUTF8(const std::wstring& wstr, std::string* outStr);
		EZUI_API void UTF8ToANSI(const std::string& str, std::string* outStr);
		EZUI_API void UTF8ToUnicode(const std::string& str, std::wstring* outStr);
		//
		EZUI_API void Tolower(std::string* str_in_out);
		EZUI_API void Toupper(std::string* str_in_out);
		EZUI_API void Erase(std::string* str_in_out, char ch);
		EZUI_API void Replace(std::string* str_in_out, char oldChar, char newChar);
		EZUI_API size_t Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll = true);
		EZUI_API void Split(const std::string& str_in, const std::string& ch, std::vector<std::string>* strs_out);

		/// <summary>
		/// 数字转为字符串(保留小数位数)
		/// </summary>
		/// <param name="number">数字</param>
		/// <param name="precision">保留小数位数</param>
		/// <returns></returns>
		EZUI_API String ToString(double number, int precision);
		//将指针转换成16进制字符串
		EZUI_API String ToString(void* ptr);
		//转换成"true"或"false"字符串
		EZUI_API String ToString(bool v);
		inline String ToString(char v) { return std::to_string((long long)v); }
		inline String ToString(signed char v) { return std::to_string((long long)v); }
		inline String ToString(unsigned char v) { return std::to_string((unsigned long long)v); }
		inline String ToString(short v) { return std::to_string((long long)v); }
		inline String ToString(unsigned short v) { return std::to_string((unsigned long long)v); }
		inline String ToString(int v) { return std::to_string((long long)v); }
		inline String ToString(unsigned int v) { return std::to_string((unsigned long long)v); }
		inline String ToString(long v) { return std::to_string((long long)v); }
		inline String ToString(unsigned long v) { return std::to_string((unsigned long long)v); }
		inline String ToString(long long v) { return std::to_string((long long)v); }
		inline String ToString(unsigned long long v) { return std::to_string((unsigned long long)v); }
		inline String ToString(float v) { return std::to_string((long double)v); }
		inline String ToString(double v) { return std::to_string((long double)v); }
		inline String ToString(long double v) { return std::to_string((long double)v); }
		//-----------------------------------------------Copy End-----------------------------------------------
	};
	typedef detail::String UIString;
	using detail::ToString;
};
