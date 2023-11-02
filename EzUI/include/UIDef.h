#pragma once

#include <list>
#include <vector>
#include <map>
#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <random>

#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <vcruntime.h>
#include <commdlg.h>
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <Ime.h>
#include <shellapi.h>
#include <time.h>
#include <sys/timeb.h>
#include <WinUser.h>
#include <objbase.h>
#include <shlwapi.h>
#include <objidl.h>
#include <commctrl.h>
//#include <shellscalingapi.h>

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

//����ڱ�������Ϣ ����GUI����ڲ�ͨѶ
#define WM_GUI_SYSTEM WM_USER
#define WM_GUI_APP WM_APP 
//��չ��Ϣ ��WM_GUI_SYSTEM��Ϣ�е�wParam����������
#define WM_GUI_INVOKE 0x01
#define WM_GUI_BEGININVOKE 0x02

#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#else
#define UI_EXPORT
#endif // _WINDLL

//�������Ⱦ��ʽֻ��ѡһ��
#define USED_DIRECT2D 1  //DX���� ���ܺ� �ڴ�ռ�ø�
