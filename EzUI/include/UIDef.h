#pragma once

#include <list>
#include <vector>
#include <functional>
#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <future>

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
#include <objidl.h>
#pragma comment(lib,"Uuid.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib,"odbc32.lib")
//#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib, "Msimg32.lib")
#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(expr)  _ASSERTE(expr)
#else
#define ASSERT(expr)  ((void)0)
#endif
#endif

#undef LoadCursor
extern HCURSOR LoadCursor(HINSTANCE hInstance, LPCTSTR lpCursorName);
inline HCURSOR LoadCursor(HINSTANCE hInstance, LPCTSTR lpCursorName) {
#ifdef UNICODE
	return ::LoadCursorW(NULL, lpCursorName);
#else
	return ::LoadCursorA(NULL, lpCursorName);
#endif // UNICODE
}

#ifndef GCL_HCURSOR
#define GCL_HCURSOR -12
#endif

#ifdef _WIN64
#define UI_SET_USERDATA(hWnd,data)  SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)data);
#define UI_GET_USERDATA(hwnd) GetWindowLongPtrW(hwnd, GWLP_USERDATA);
#else  
#define UI_SET_USERDATA(hWnd,data)  SetWindowLongW(hWnd, GWLP_USERDATA, (LONG)data);
#define UI_GET_USERDATA(hwnd) GetWindowLongW(hwnd, GWLP_USERDATA);
#endif

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define UI_NOTIFYICON WM_USER+0x01 //

#define WM_UIMESSAGE  WM_USER+20   
#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#else
#define UI_EXPORT
#endif // _WINDLL

//下面的渲染方式只能选一个
#define USED_DIRECT2D 1  //DX绘制 性能好 内存占用高
