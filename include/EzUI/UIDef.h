#pragma once

#include <list>
#include <vector>
#include <map>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <condition_variable>

#include <windows.h>

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#endif // !GET_X_LPARAM

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#endif // !GET_Y_LPARAM

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(expr)  _ASSERTE(expr)
#else
#define ASSERT(expr)  ((void)0)
#endif
#endif

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

//框架内保留的消息 用于GUI框架内部通讯
#define WM_GUI_SYSTEM WM_USER
#define WM_GUI_APP WM_APP 
//扩展消息 在WM_GUI_SYSTEM消息中的wParam参数中体现
#define WM_GUI_INVOKE 0x01
#define WM_GUI_BEGININVOKE 0x02

#ifdef _WINDLL

#define UI_EXPORT  __declspec(dllexport) 
#define	UI_VAR_EXPORT __declspec(dllexport) 

#else

#define UI_EXPORT 
#define UI_VAR_EXPORT __declspec(dllimport)

#endif // _WINDLL

#define EZUI_WINDOW_CLASS L"EzUI_Window" //基础窗口类名
#define EZUI_INVOKER_WINDOW_CLASS L"EzUI_InvokerWindow" //用于线程同步的窗口类名
#define EZUI_FLOAT_MAX 16777216.0f //最后一个可以精确表示的整数
#define EZUI_FLOAT_EPSILON 1e-6f  // 浮点误差阈值
//下面的渲染方式只能选一个
#define USED_DIRECT2D 1  //DX绘制 性能好 内存占用高

//生成枚举类常用操作符
#define EZUI_ENUM_OPERATORS(ENUM_TYPE, BASE_TYPE)         \
inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) | static_cast<BASE_TYPE>(b)); \
}                                                           \
inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) & static_cast<BASE_TYPE>(b)); \
}                                                            \
inline ENUM_TYPE operator~(ENUM_TYPE a) {                        \
    return static_cast<ENUM_TYPE>(~static_cast<BASE_TYPE>(a));   \
}                                                              \
inline ENUM_TYPE operator^(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) ^ static_cast<BASE_TYPE>(b)); \
}                                                               \
inline ENUM_TYPE& operator|=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a | b;                                                   \
    return a;                                                    \
}                                                              \
inline ENUM_TYPE& operator&=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a & b;                                                   \
    return a;                                                    \
}                                                               \
inline ENUM_TYPE& operator^=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a ^ b;                                                   \
    return a;                                                    \
}
