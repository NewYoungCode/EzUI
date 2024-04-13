#include "EString.h"
namespace EzUI {
	size_t EString::Length() const {
		auto* p = this->c_str();
		size_t pos = 0, count = 0;
		while (p[pos] && pos < this->size()) {
			if ((p[pos] & 0xc0) != 0x80) {
				++count;
			}
			++pos;
		}
		return count;
	}
	EString::EString() {}
	EString::EString(const std::string& str)noexcept :std::string(str) {}
	EString::EString(const char* szbuf)noexcept :std::string(szbuf) {}
	EString::EString(const wchar_t* szbuf)noexcept {
		if (szbuf == NULL)return;
		EString::UnicodeToUTF8(szbuf, this);
	}
	EString::EString(const std::wstring& wstr)noexcept {
		EString::UnicodeToUTF8(wstr, this);
	}

	//常用函数
	EString EString::Erase(const char& _char)const {
		EString newStr(*this);
		EString::Erase(&newStr, _char);
		return newStr;
	}
	std::vector<std::string> EString::Split(const EString& ch_)const {
		std::vector<std::string> arr;
		EString::Split(*this, ch_, &arr);
		return arr;
	}
	EString EString::Replace(const EString& oldText, const EString& newText, bool replaceAll) const
	{
		EString newStr = *this;
		EString::Replace(&newStr, oldText, newText, replaceAll);
		return newStr;
	}
	EString EString::Tolower() const
	{
		EString str(*this);
		EString::Tolower(&str);
		return str;
	}
	EString EString::Toupper() const
	{
		EString str(*this);
		EString::Toupper(&str);
		return str;
	}
	bool EString::operator==(const wchar_t* szbuf)const
	{
		std::string u8str;
		EString::UnicodeToUTF8(szbuf, &u8str);
		return (*this == u8str);
	}
	bool EString::operator==(const std::wstring& wStr)const
	{
		std::string u8str;
		EString::UnicodeToUTF8(wStr, &u8str);
		return (*this == u8str);
	}
	std::wstring EString::unicode() const {
		std::wstring wstr;
		EString::UTF8ToUnicode(*this, &wstr);
		return wstr;
	}
	std::string EString::ansi() const {
		std::string str;
		EString::UTF8ToANSI(*this, &str);
		return str;
	}

	void EString::AnyToUnicode(const std::string& src_str, UINT codePage, std::wstring* out_wstr) {
		std::wstring& wstrCmd = *out_wstr;
		int bytes = ::MultiByteToWideChar(codePage, 0, src_str.c_str(), src_str.size(), NULL, 0);
		wstrCmd.resize(bytes);
		bytes = ::MultiByteToWideChar(codePage, 0, src_str.c_str(), src_str.size(), const_cast<wchar_t*>(wstrCmd.c_str()), wstrCmd.size());
	}
	void EString::UnicodeToAny(const std::wstring& wstr, UINT codePage, std::string* out_str) {
		std::string& strCmd = *out_str;
		int bytes = ::WideCharToMultiByte(codePage, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		strCmd.resize(bytes);
		bytes = ::WideCharToMultiByte(codePage, 0, wstr.c_str(), wstr.size(), const_cast<char*>(strCmd.c_str()), strCmd.size(), NULL, NULL);
	}

	//以下是静态函数
	void EString::ANSIToUniCode(const std::string& str, std::wstring* outStr)
	{
		AnyToUnicode(str, ::GetACP(), outStr);
	}
	void EString::UnicodeToANSI(const std::wstring& wstr, std::string* outStr)
	{
		UnicodeToAny(wstr, ::GetACP(), outStr);
	}

	void EString::GBKToUTF8(const std::string& str, std::string* outStr) {
		const int gbkCodePage = 936;
		std::wstring wstr;
		AnyToUnicode(str, gbkCodePage, &wstr);
		UnicodeToUTF8(wstr, outStr);
	}

	void EString::UTF8ToGBK(const std::string& str, std::string* outStr) {
		const int gbkCodePage = 936;
		std::wstring wstr;
		UTF8ToUnicode(str, &wstr);
		UnicodeToAny(wstr, gbkCodePage, outStr);
	}

	void EString::ANSIToUTF8(const std::string& str, std::string* outStr)
	{
		UINT codePage = ::GetACP();
		if (codePage == CP_UTF8) {
			*outStr = str;//如果本身就是utf8则不需要转换
			return;
		}
		std::wstring wstr;
		AnyToUnicode(str, codePage, &wstr);
		UnicodeToUTF8(wstr, outStr);
	}
	void EString::UTF8ToANSI(const std::string& str, std::string* outStr) {
		UINT codePage = ::GetACP();
		if (codePage == CP_UTF8) {
			*outStr = str;//如果本身就是utf8则不需要转换
			return;
		}
		std::wstring wstr;
		UTF8ToUnicode(str, &wstr);
		UnicodeToAny(wstr, codePage, outStr);
	}
	void EString::UnicodeToUTF8(const std::wstring& wstr, std::string* outStr)
	{
		UnicodeToAny(wstr, CP_UTF8, outStr);
	}
	void EString::UTF8ToUnicode(const std::string& str, std::wstring* outStr) {
		AnyToUnicode(str, CP_UTF8, outStr);
	}


	void EString::Tolower(std::string* str_in_out)
	{
		std::string& str = *str_in_out;
		for (size_t i = 0; i < str.size(); ++i)
		{
			char& ch = (char&)str.c_str()[i];
			if (ch >= 65 && ch <= 90) {
				ch += 32;
			}
		}
	}
	void EString::Toupper(std::string* str_in_out)
	{
		std::string& str = *str_in_out;
		for (size_t i = 0; i < str.size(); ++i)
		{
			char& ch = (char&)str.c_str()[i];
			if (ch >= 97 && ch <= 122) {
				ch -= 32;
			}
		}
	}
	void EString::Erase(std::string* str_in_out, const char& _char) {
		const EString& self = *str_in_out;
		char* bufStr = new char[self.size() + 1] { 0 };
		size_t pos = 0;
		for (auto& it : self) {
			if (_char == it)continue;
			bufStr[pos] = it;
			++pos;
		}
		*str_in_out = bufStr;
		delete[] bufStr;
	}
	void EString::Replace(std::string* str_in_out, const std::string& oldText, const std::string& newText, bool replaceAll)
	{
		std::string& newStr = *str_in_out;
		size_t pos;
		pos = newStr.find(oldText);
		while (pos != std::string::npos)
		{
			newStr.replace(pos, oldText.size(), newText);
			pos = newStr.find(oldText);
			if (!replaceAll) {
				break;
			}
		}
	}
	void EString::Split(const std::string& str_in, const std::string& ch_, std::vector<std::string>* strs_out) {

		std::vector<std::string>& arr = *strs_out;
		arr.clear();
		if (str_in.empty()) return;

		std::string buf = str_in;
		size_t pos = buf.find(ch_);
		if (pos == std::string::npos) {
			arr.push_back(buf);
			return;
		}
		while (pos != std::string::npos) {
			auto item = buf.substr(0, pos);
			if (!item.empty()) {
				arr.push_back(item);
			}
			buf = buf.erase(0, pos + ch_.size());
			pos = buf.find(ch_);
			if (pos == std::string::npos) {
				if (!buf.empty()) {
					arr.push_back(buf);
				}
			}
		}
	}

	EString EString::ToString(double number, int keepBitSize) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(keepBitSize) << number;
		return oss.str();
	}
};