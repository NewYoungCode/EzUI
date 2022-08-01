#include "EzUI.h"
size_t EString::length() const {
	auto* p = this->c_str();
	size_t pos = 0, count = 0;
	while (p[pos] && pos < this->size()) {
		if ((p[pos] & 0xc0) != 0x80) {
			count++;
		}
		pos++;
	}
	return count;
}
EString::EString() {}
EString::EString(const std::string& str, int codePage) :std::string() {
	if ((codePage == EString::ANSI) && (::GetACP() != CP_UTF8)) {
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, NULL, NULL, NULL, NULL);
		this->resize(nLen);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, (char*)this->c_str(), nLen, NULL, NULL);
		delete[]pwBuf;
	}
	else {
		this->resize(str.size());
		::memcpy((void*)c_str(), str.c_str(), str.size());
	}
}
EString::EString(const char* szbuf, int codePage) {
	if (szbuf == NULL)return;
	size_t len = ::strlen(szbuf);
	if ((codePage == EString::ANSI) && (::GetACP() != CP_UTF8)) {
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, szbuf, len, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, szbuf, len, pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, NULL, NULL, NULL, NULL);
		this->resize(nLen);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, (char*)this->c_str(), nLen, NULL, NULL);
		delete[]pwBuf;
	}
	else
	{
		this->resize(len);
		::memcpy((void*)c_str(), szbuf, len);
	}
}
EString::EString(const wchar_t* szbuf) {
	if (szbuf == NULL)return;
	size_t len = ::lstrlenW(szbuf);
	int bytes = ::WideCharToMultiByte(CP_UTF8, 0, szbuf, len, NULL, 0, NULL, NULL);
	this->resize(bytes);
	WideCharToMultiByte(CP_UTF8, 0, szbuf, len, (char*)this->c_str(), bytes, NULL, NULL);
}
EString::EString(const std::wstring& wstr) {
	int bytes = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	this->resize(bytes);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), (char*)this->c_str(), bytes, NULL, NULL);
}

EString EString::Erase(CHAR _char)const {
	const EString& self = *this;
	CHAR* bufStr = new CHAR[self.size()]{ 0 };
	size_t pos = 0;
	for (auto& it : self) {
		if (_char == it)continue;
		bufStr[pos] = it;
		pos++;
	}
	EString newStr(bufStr);
	delete bufStr;
	return newStr;
}
std::vector<EString> EString::Split(const EString& ch_)const {

	const EString& str = *this;

	std::vector<EString> arr;
	if (str.empty()) return arr;
	EString buf = str;
	size_t pos = buf.find(ch_);
	if (pos == EString::npos) {
		arr.push_back(str);
		return arr;
	}
	for (; pos != EString::npos;) {
		auto item = buf.substr(0, pos);
		if (!item.empty()) {
			arr.push_back(item);
		}
		buf = buf.erase(0, pos + ch_.size());
		pos = buf.find(ch_);
		if (pos == std::string::npos) {
			arr.push_back(buf);
		}
	}
	return arr;
}
EString EString::Replace(const EString& oldText, const EString& newText) const
{
	EString newStr = *this;
	EString::Replace(newStr, oldText, newText);
	return newStr;
}
std::wstring EString::utf16() const {
	int textlen = MultiByteToWideChar(CP_UTF8, 0, this->c_str(), this->size(), NULL, 0);
	std::wstring wstr;
	wstr.resize(textlen);
	MultiByteToWideChar(CP_UTF8, 0, this->c_str(), this->size(), (WCHAR*)wstr.c_str(), textlen);
	return wstr;
}
