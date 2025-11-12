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
#include <atomic>
#include <condition_variable>
#include <limits>
#include <cstdio>

#include <windows.h>

namespace ezui {
	namespace detail {
		inline void EZUI_OutputAssert(const wchar_t* expr, const wchar_t* file, int line)
		{
			const wchar_t* safeFile = file ? file : L"(unknown)";
			const wchar_t* safeExpr = expr ? expr : L"(null expr)";
			// 计算长度
			int len = _snwprintf(NULL, 0, L"\n[ASSERT] %s(%d): %s\n\n", safeFile, line, safeExpr);
			if (len < 0) len = 256; // fallback 防止不支持 0 输出的旧 CRT
			// 分配缓冲区
			size_t bufSize = static_cast<size_t>(len) + 1;
			std::wstring buf(bufSize, L'\0');
			// 格式化内容
			_snwprintf(&buf[0], bufSize, L"\n[ASSERT] %s(%d): %s\n\n", safeFile, line, safeExpr);
			// 输出到控制台
			std::fwprintf(stderr, L"%s", buf.c_str());
			std::fflush(stderr);
			// 输出到调试器
			OutputDebugStringW(buf.c_str());
		}
	};
};

#define ASSERT(expr) \
    (void)( \
        (!!(expr)) || \
        ( \
            (ezui::detail::EZUI_OutputAssert(L#expr, _CRT_WIDE(__FILE__), __LINE__), 0) || \
            (DebugBreak(), 0) \
        ) \
    )

#define ASSERT_MSG(msg_p_wchar_t) \
    (void)( \
        (ezui::detail::EZUI_OutputAssert(msg_p_wchar_t, _CRT_WIDE(__FILE__), __LINE__), 0) || \
        (DebugBreak(), 0) \
    )


#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#endif // !GET_X_LPARAM

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#endif // !GET_Y_LPARAM

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
#define WM_GUI_LAYERED_PAINT 0x01
#define WM_GUI_INVOKE 0x02
#define WM_GUI_BEGININVOKE 0x03

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
}                                                                 \
inline bool EnumExist(ENUM_TYPE flags, ENUM_TYPE flag) {          \
    return (static_cast<BASE_TYPE>(flags) & static_cast<BASE_TYPE>(flag)) != 0; \
}                                                                       \
inline ENUM_TYPE& EnumAdd(ENUM_TYPE& flags, ENUM_TYPE flag) {\
    flags = static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(flags) | static_cast<BASE_TYPE>(flag)); \
    return flags;                                                        \
}                                                                        \
inline ENUM_TYPE& EnumRemove(ENUM_TYPE& flags, ENUM_TYPE flag) {\
    flags = static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(flags) & ~static_cast<BASE_TYPE>(flag)); \
    return flags;                                                        \
}
