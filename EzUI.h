#pragma once
#include <list>
#include <vector>
#include <functional>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <map>
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
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
	bool valid = false;//字段内容是否有效
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
#define DEBUGPAINT //调试模式绘制边框,布局信息 宏
#define DEBUGLOG //调试打印输出
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

#define WM_NOTIFYICON WM_USER+0x01 //左下角托盘消息
#define WM_CONTROL_DELETE WM_USER+0x02 //控件删除的时候发生此消息
#define WM_CONTROL_REFRESH WM_USER+0x03 //窗口刷新的消息
//WM_USER ~ 0x7FFF
//框架内定义的鼠标鼠标消息
//0x04~0x0b鼠标消息
//0x0c~0x0e键盘消息(按下,弹起,WM_CHAR)

#define WM_UIMESSAGE  WM_USER+20   //用户额外的消息 UI保留20个消息

#ifdef UNICODE
//std::wstring
using EString = std::wstring;
#else
//std::string
using EString = std::string;
#endif

namespace String {
	inline std::wstring ANSIToUniCode(const std::string &str)
	{
		int bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		std::wstring wstrCmd;
		wstrCmd.resize(bytes);
		bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), const_cast<wchar_t*>(wstrCmd.c_str()), wstrCmd.size());
		return wstrCmd;
	}

	inline std::vector<std::string> Split(const std::string& str, const std::string& ch_) {
		std::vector<std::string> arr;
		if (str.empty()) return arr;
		std::string buf = str;
		size_t pos = buf.find(ch_);
		if (pos == std::string::npos) {
			arr.push_back(str);
			return arr;
		}
		for (; pos != std::string::npos;) {
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
		return ::MessageBox(::GetActiveWindow(), text.c_str(), title.c_str(), mButton);
	}
}

namespace Debug {
	template<typename ...T>
	inline void Log(const EString&formatStr, T ...args) {
#ifdef DEBUGLOG
		TCHAR buf[256]{ 0 };
#ifdef UNICODE
		auto count = swprintf_s((buf), 255, formatStr.c_str(), std::forward<T>(args)...);
#else
		auto count = sprintf_s((buf), 255, formatStr.c_str(), std::forward<T>(args)...);
#endif
		buf[count] = '\n';
		buf[count + 1] = NULL;
		OutputDebugString(buf);
#endif
	}
};

struct StopWatch {
private:
	std::chrono::system_clock::time_point beg_t;
public:
	StopWatch() {
		beg_t = std::chrono::system_clock::now();    //起始时间
	}
	time_t ElapsedMilliseconds() {
		auto end_t = std::chrono::system_clock::now();    //结束时间
		std::chrono::duration<double> diff = end_t - beg_t;//时间间隔s
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//时间间隔ms
	}
};



namespace Dialog {
	inline EString OpenFileDialog(const EString&title, const TCHAR*filter, const EString&defaultPath) {
		HWND hwnd = ::GetForegroundWindow();//获取系统前景窗口
		OPENFILENAME ofn = { 0 };
		TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名
		ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
		ofn.hwndOwner = hwnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
		ofn.lpstrFilter = filter;//TEXT("所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//设置过滤
		ofn.nFilterIndex = 1;//过滤器索引
		ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
		ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
		ofn.lpstrInitialDir = defaultPath.c_str();//初始目录为默认
		ofn.lpstrTitle = title.c_str();//使用系统默认标题留空即可
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
		GetOpenFileName(&ofn);
		return strFilename;
	}

	//
	//#include<windows.h>
	////打开保存文件对话框
	//#include<Commdlg.h>
	//
	////选择文件夹对话框
	//#include<Shlobj.h>
	//#pragma comment(lib,"Shell32.lib")
	//
	//int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	//	_In_opt_ HINSTANCE hPrevInstance,
	//	_In_ LPTSTR    lpCmdLine,
	//	_In_ int       nCmdShow)
	//{
	//loop:
	//	OPENFILENAME ofn = { 0 };
	//	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名
	//	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
	//	ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	//	ofn.lpstrFilter = TEXT("所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//设置过滤
	//	ofn.nFilterIndex = 1;//过滤器索引
	//	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
	//	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
	//	ofn.lpstrInitialDir = NULL;//初始目录为默认
	//	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可
	//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
	//	if (GetOpenFileName(&ofn))
	//	{
	//		MessageBox(NULL, strFilename, TEXT("选择的文件"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("请选择一个文件"), NULL, MB_ICONERROR);
	//		goto loop;
	//	}
	//
	//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//目录必须存在，覆盖文件前发出警告
	//	ofn.lpstrTitle = TEXT("保存到");//使用系统默认标题留空即可
	//	ofn.lpstrDefExt = TEXT("cpp");//默认追加的扩展名
	//	if (GetSaveFileName(&ofn))
	//	{
	//		MessageBox(NULL, strFilename, TEXT("保存到"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("请输入一个文件名"), NULL, MB_ICONERROR);
	//	}
	//
	//	TCHAR szBuffer[MAX_PATH] = { 0 };
	//	BROWSEINFO bi = { 0 };
	//	bi.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	//	bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区
	//	bi.lpszTitle = TEXT("选择一个文件夹");//标题
	//	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	//	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	//	if (SHGetPathFromIDList(idl, szBuffer)) {
	//		MessageBox(NULL, szBuffer, TEXT("你选择的文件夹"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("请选择一个文件夹"), NULL, MB_ICONERROR);
	//	}
	//
	//	return 0;
	//}
	//

}
