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
#include <functional>
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
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

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

#define UI_NOTIFYICON WM_USER+0x01 //
#define UI_CONTROL_DELETE WM_USER+0x02 //
#define UI_PAINT WM_USER+0x03 //

#define WM_UIMESSAGE  WM_USER+20   
#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#else
#define UI_EXPORT
#endif // _WINDLL

#define ui EzUI
namespace EzUI {
	template<typename T>
	struct Tuple {
	public:
		bool valid = false;
		T value = NULL;
		Tuple() :valid(false) {
		}
		Tuple(const T& value) {
			this->value = value;
			valid = true;
		}
		operator T& () {
			return value;
		}
		T& operator->() {
			return value;
		}
	};

	template<typename T>
	struct SafeObject {
	private:
		SafeObject(const SafeObject& right) {}
		SafeObject& operator=(const SafeObject& right) {}
	public:
		T* ptr = NULL;
		SafeObject(T* _ptr) {
			ptr = _ptr;
		}
		operator T* () {
			return ptr;
		}
		~SafeObject() {
			if (ptr) delete ptr;
		}
	};
	struct WindowData {
		void* Window = NULL;
		void* HoverStyles = NULL;
		void* ActiveStyles = NULL;
		bool Debug = false;
		std::function<void(void*)> InvalidateRect = NULL;
		std::function<void()> UpdateWindow = NULL;
		std::function<bool(UINT, WPARAM, LPARAM)> Notify = NULL;
	};

#define UI_Float Tuple<float>
#define UI_Int Tuple<int>
#define UI_UInt Tuple<size_t>

	class UI_EXPORT EString :public std::string {
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
		static size_t  Replace(EString& str, const EString& oldText, const EString& newText)
		{
			EString& newStr = str;
			size_t pos;
			pos = str.find(oldText);
			size_t count = 0;
			for (; pos != std::string::npos;) {
				newStr.replace(pos, oldText.size(), newText);
				count++;
				pos = newStr.find(oldText);
			}
			return count;
		}

		EString Tolower()const;
		EString Toupper()const;

	};
#define utf8(szbuf)  EString(szbuf,EString::ANSI)
	//#define utf8(szbuf)  EString(szbuf,EString::UTF8)

	namespace Debug {
		template<typename ...T>
		inline void Log(const EString& formatStr, T ...args) {
#ifdef DEBUGLOG
			WCHAR buf[1024]{ 0 };
			auto count = swprintf_s((buf), 1024, formatStr.utf16().c_str(), std::forward<T>(args)...);
			buf[count] = '\n';
			buf[count + 1] = NULL;
			OutputDebugStringW(buf);
#endif
		}
	};

	struct StopWatch {
	private:
		std::chrono::system_clock::time_point beg_t;
	public:
		StopWatch() {
			beg_t = std::chrono::system_clock::now();    //
		}
		static time_t Now() {
			auto nowTime = std::chrono::system_clock::now();    //
			return std::chrono::system_clock::to_time_t(nowTime);
		}
		time_t ElapsedMilliseconds() {
			auto end_t = std::chrono::system_clock::now();    //
			std::chrono::duration<double> diff = end_t - beg_t;//
			return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		}
	};

	namespace Dialog {
		//inline EString OpenFileDialog(HWND oWner, const EString&title, const EString&filter, const EString&defaultPath) {
		//	OPENFILENAMEW ofn = { 0 };
		//	WCHAR strFilename[MAX_PATH] = { 0 };//
		//	ofn.lStructSize = sizeof(OPENFILENAME);//
		//	ofn.hwndOwner = oWner;//
		//	ofn.lpstrFilter = filter.utf16().c_str();//
		//	ofn.nFilterIndex = 1;//
		//	ofn.lpstrFile = strFilename;//
		//	ofn.nMaxFile = sizeof(strFilename);//
		//	ofn.lpstrInitialDir = defaultPath.utf16().c_str();//
		//	ofn.lpstrTitle =  title.utf16().c_str();//
		//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//
		//	GetOpenFileNameW(&ofn);
		//	return strFilename;
		//}
		//inline EString OpenFileDialog(HWND oWner, const EString& title, const EString& filter, const EString& defaultPath) {
		//	OPENFILENAME ofn = { 0 };
		//	CHAR strFilename[MAX_PATH] = { 0 };//
		//	ofn.lStructSize = sizeof(OPENFILENAME);//
		//	ofn.hwndOwner = oWner;//
		//	ofn.lpstrFilter = filter.c_str();//
		//	ofn.nFilterIndex = 1;//
		//	ofn.lpstrFile = strFilename;//
		//	ofn.nMaxFile = sizeof(strFilename);//
		//	ofn.lpstrInitialDir = defaultPath.c_str();//
		//	ofn.lpstrTitle = title.c_str();//
		//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//
		//	GetOpenFileName(&ofn);
		//	return strFilename;
		//}
	}
};