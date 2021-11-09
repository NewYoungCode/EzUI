#pragma once
#include <list>
#include <vector>
#include <functional>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN            
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <vcruntime.h>
#include <commdlg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Ime.h>
#include <shellapi.h>
#include <time.h>
#include <sys/timeb.h>
#include <WinUser.h>
#include <objbase.h>
#define UI namespace EzUI

#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#else
#define UI_EXPORT
#endif // _WINDLL

template<typename T>
struct Tuple {
public:
	bool valid = false;
	T value = NULL;
	Tuple() :valid(false) {
	}
	Tuple(const T&value) {
		this->value = value;
		valid = true;
	}
	operator T&() {
		return value;
	}
	T& operator->() {
		return value;
	}
};

template<typename T>
struct SafeObject {
private:
	SafeObject(const SafeObject&right) {}
	SafeObject& operator=(const SafeObject&right) {}
public:
	T* ptr = NULL;
	SafeObject(T* _ptr) {
		ptr = _ptr;
	}

	~SafeObject() {
		if (ptr) delete ptr;
	}
};


#define UI_Float Tuple<float>
#define UI_Int Tuple<int>
#define UI_UInt Tuple<size_t>

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#define DEBUGPAINT //
#define DEBUGLOG //
#endif

#ifndef GCL_HCURSOR
#define GCL_HCURSOR -12
#endif

#ifdef _WIN64
#define CPU64
#define UI_SetUserData(hWnd,data)  SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);
#define UI_GetUserData(hwnd) GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else  
#define CPU86
#define UI_SetUserData(hWnd,data)  SetWindowLong(hWnd, GWLP_USERDATA, (LONG)data);
#define UI_GetUserData(hwnd) GetWindowLong(hwnd, GWLP_USERDATA);
#endif
//
#pragma warning(disable:4244)
#pragma warning(disable:4267)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define UI_CLASSNAME TEXT("EzUI_Win32")
#define UI_NotifyIcon_CLASSNAME TEXT("EzUI_NotifyIcon_Class")

#define WM_NOTIFYICON WM_USER+0x01 //
#define WM_CONTROL_DELETE WM_USER+0x02 //
#define WM_CONTROL_REFRESH WM_USER+0x03 //

#define WM_UIMESSAGE  WM_USER+20   
//utf8
class EString :public std::string {
public:
	enum
	{
		ANSI = CP_ACP,
		UTF8 = CP_UTF8
	};
public:
	size_t length() const {
		const char*s = this->c_str();
		int i = 0, j = 0;
		while (s[i]) {
			if ((s[i] & 0xc0) != 0x80) {
				j++;
			}
			i++;
		}
		return j;
	}
	EString() {}
	EString(const std::string&str, int codePage = EString::UTF8) :std::string() {
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
	EString(const char*szbuf, int codePage = EString::UTF8) {
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
	EString(const wchar_t*szbuf) {
		size_t len = ::lstrlenW(szbuf);
		int bytes = ::WideCharToMultiByte(CP_UTF8, 0, szbuf, len, NULL, 0, NULL, NULL);
		this->resize(bytes);
		WideCharToMultiByte(CP_UTF8, 0, szbuf, len, (char*)this->c_str(), bytes, NULL, NULL);
	}
	EString(const std::wstring*wstr) {
		int bytes = ::WideCharToMultiByte(CP_UTF8, 0, wstr->c_str(), wstr->size(), NULL, 0, NULL, NULL);
		this->resize(bytes);
		WideCharToMultiByte(CP_UTF8, 0, wstr->c_str(), wstr->size(), (char*)this->c_str(), bytes, NULL, NULL);
	}
	std::wstring utf16() const {
		int textlen = MultiByteToWideChar(CP_UTF8, 0, this->c_str(), this->size(), NULL, 0);
		std::wstring wstr;
		wstr.resize(textlen);
		MultiByteToWideChar(CP_UTF8, 0, this->c_str(), this->size(), (WCHAR*)wstr.c_str(), textlen);
		return wstr;
	}
};
#define utf8(szbuf)  EString(szbuf,EString::ANSI)

namespace String {
	inline std::wstring ANSIToUniCode(const std::string &str)
	{
		int bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		std::wstring wstrCmd;
		wstrCmd.resize(bytes);
		bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), const_cast<wchar_t*>(wstrCmd.c_str()), wstrCmd.size());
		return wstrCmd;
	}
	inline std::string UnicodeToANSI(const std::wstring &wstr)
	{
		int bytes = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		std::string strCmd;
		strCmd.resize(bytes);
		bytes = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), const_cast<char*>(strCmd.data()), strCmd.size(), NULL, NULL);
		return strCmd;
	}
	inline std::string UnicodeToUTF8(const std::wstring &wstr)
	{
		int bytes = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		std::string strCmd;
		strCmd.resize(bytes);
		bytes = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), const_cast<char*>(strCmd.data()), strCmd.size(), NULL, NULL);
		return strCmd;
	}
	inline 	std::string UTF8ToANSI(const std::string& str)
	{
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string strRet = pBuf;

		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return strRet;
	}
	inline std::string ANSIToUTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string strRet(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return strRet;
	}

	inline std::vector<EString> Split(const EString& str, const EString& ch_) {
		std::vector<EString> arr;
		if (str.empty()) return arr;
		EString buf = str;
		size_t pos = buf.find(ch_);
		if (pos == EString::npos) {
			arr.push_back(str);
			return arr;
		}
		for (; pos != EString::npos;) {
			arr.push_back(buf.substr(0, pos));
			buf = buf.erase(0, pos + ch_.size());
			pos = buf.find(ch_);
			if (pos == std::string::npos) {
				arr.push_back(buf);
			}
		}
		return arr;
	}
}

namespace MsgBox {
	inline int Show(const EString&text, const EString&title = TEXT(""), int mButton = NULL) {
		return ::MessageBoxW(::GetActiveWindow(), text.utf16().c_str(), text.utf16().c_str(), mButton);
	}
}

namespace Debug {
	template<typename ...T>
	inline void Log(const EString&formatStr, T ...args) {
//#ifdef DEBUGLOG
//		WCHAR buf[256]{ 0 };
//		auto count = swprintf_s((buf), 255, formatStr.c_str(), std::forward<T>(args)...);
//		buf[count] = '\n';
//		buf[count + 1] = NULL;
//		OutputDebugStringW(buf);
//#endif
	}
};

struct StopWatch {
private:
	std::chrono::system_clock::time_point beg_t;
public:
	StopWatch() {
		beg_t = std::chrono::system_clock::now();    //
	}
	time_t ElapsedMilliseconds() {
		auto end_t = std::chrono::system_clock::now();    //
		std::chrono::duration<double> diff = end_t - beg_t;//
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
	}
};



namespace Dialog {
	inline EString OpenFileDialog(HWND oWner, const EString&title, const EString&filter, const EString&defaultPath) {
		OPENFILENAMEW ofn = { 0 };
		WCHAR strFilename[MAX_PATH] = { 0 };//
		ofn.lStructSize = sizeof(OPENFILENAME);//
		ofn.hwndOwner = oWner;//
		ofn.lpstrFilter = filter.utf16().c_str();//
		ofn.nFilterIndex = 1;//
		ofn.lpstrFile = strFilename;//
		ofn.nMaxFile = sizeof(strFilename);//
		ofn.lpstrInitialDir = defaultPath.utf16().c_str();//
		ofn.lpstrTitle = title.utf16().c_str();//
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//
		GetOpenFileNameW(&ofn);
		return strFilename;
	}


}
