#pragma once

// Windows
#if defined(_WIN32) || defined(_WIN64)
//windows平台
#define EZUI_OS_WIN 1
// Apple (macOS / iOS)
#elif defined(__APPLE__) && defined(__MACH__)
//苹果平台
#define EZUI_OS_APPLE 1
// Linux
#elif defined(__linux__)
//Linux平台
#define EZUI_OS_LINUX 1
#endif


//调试模式宏
#if !defined(NDEBUG)
#if !defined(_DEBUG)
#error "Debug build detected (NDEBUG not defined), but _DEBUG is not defined. Please add _DEBUG to your build configuration."
#endif
#define DEBUG 1
#define EZUI_DEBUG 1
#endif


#include <cstdint>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <ctime>


//平台相关头文件
#ifdef EZUI_OS_WIN
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <assert.h>

#if defined(_MSC_VER) && _MSC_VER <= 1600
#define EZUI_COMPILER_VS2010 1
#endif

//#define EZUI_COMPILER_VS2010 1

#ifdef EZUI_COMPILER_VS2010
#define EZUI_OVERRIDE 
#define EZUI_NOEXCEPT 
#define EZUI_FINAL 
#define WM_DPICHANGED     0x02E0
#else
#define EZUI_OVERRIDE override
#define EZUI_NOEXCEPT noexcept
#define EZUI_FINAL final
#endif // EZUI_COMPILER_VS2010

namespace ezui {
	typedef HWND WindowHandle;

	namespace detail {
		inline void EZUI_OutputAssert(const wchar_t* expr, const wchar_t* file, unsigned int line, bool dbgBreak = true)
		{
			const wchar_t* safeFile = file ? file : L"(unknown)";
			const wchar_t* safeExpr = expr ? expr : L"(null expr)";

			std::wstring msg = dbgBreak ? L"[EzUI][ASSERT] %s(%d): %s\n" : L"[EzUI][INFO] %s(%d): %s\n";

			// 使用 swprintf 计算所需长度（支持 NULL 传入）
			int len = std::swprintf(NULL, 0, msg.c_str(), safeFile, line, safeExpr);
			if (len < 0) len = 1024; // fallback 防止格式化失败

			// 分配缓冲区
			std::vector<wchar_t> buf(static_cast<size_t>(len) + 1);
			std::swprintf(buf.data(), buf.size(), msg.c_str(), safeFile, line, safeExpr);

			// 输出到标准错误
			std::fwprintf(stderr, L"%s", buf.data());
			std::fflush(stderr);

			// 输出到调试器
			OutputDebugStringW(buf.data());

			// 触发断言
			if (dbgBreak) {
				//  如果有调试器触发断点
				if (IsDebuggerPresent()) {
					DebugBreak();
				}
				else {
					// 如果没有调试器，则弹出对话框
					int result = MessageBoxW(NULL, buf.data(), L"EZUI_OutputAssert", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON1);
					if (result == IDABORT) {
						std::abort();
					}
					else if (result == IDRETRY) {
						DebugBreak();
					}
					// IDIGNORE: 继续执行
				}
			}
		}
	};
};


//断言宏 如果表达式为假 则输出指定断言信息并且触发断点
#define EZUI_ASSERT(expression, msg_wchar_t) ((void)(                                          \
            (!!(expression)) ||                                                               \
            (::ezui::detail::EZUI_OutputAssert((msg_wchar_t), _CRT_WIDE(__FILE__), (unsigned)(__LINE__), true), 0)) \
        )

//无条件直接断言并弹出断言信息
#define EZUI_ERROR(msg_wchar_t) ((void)(                                                       \
            (::ezui::detail::EZUI_OutputAssert((msg_wchar_t), _CRT_WIDE(__FILE__), (unsigned)(__LINE__), true), 0)) \
        )

//输出指定信息但不触发断点(只打印日志)
#define EZUI_INFO(msg_wchar_t) ((void)( \
        (::ezui::detail::EZUI_OutputAssert((msg_wchar_t), _CRT_WIDE(__FILE__), (unsigned)(__LINE__), false), 0) \
    ))


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
#define EZUI_SET_USERDATA(hWnd,data)  SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)data);
#define EZUI_GET_USERDATA(hwnd) GetWindowLongPtrW(hwnd, GWLP_USERDATA);
#else  
#define EZUI_SET_USERDATA(hWnd,data)  SetWindowLongW(hWnd, GWLP_USERDATA, (LONG)data);
#define EZUI_GET_USERDATA(hwnd) GetWindowLongW(hwnd, GWLP_USERDATA);
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
#define EZUI_WM_SYSTEM WM_USER
#define EZUI_WM_APP WM_APP 
//扩展消息 在WM_GUI_SYSTEM消息中的wParam参数中体现
#define EZUI_WM_INVOKE 0x01
#define EZUI_WM_BEGININVOKE 0x02

//导出符号定义
#ifndef EZUI_API
#ifdef _WINDLL
#define EZUI_API  __declspec(dllexport) 
#else
#define EZUI_API 
#endif // _WINDLL
#endif

#endif // EZUI_OS_WIN

#define EZUI_FLOAT_MAX 16777216.0f //最后一个可以精确表示的整数
#define EZUI_FLOAT_EPSILON 1e-6f  // 浮点误差阈值

#ifdef EZUI_COMPILER_VS2010
#define EZUI_ISNAN(x) _isnan(x)
#define EZUI_ROUND(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x) - 0.5))
#else
#define EZUI_ISNAN(x) std::isnan(x)
#define EZUI_ROUND(x) std::round(x)
#endif

//最大最小值宏
#ifndef EZUI_MAX
#define EZUI_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif
//最大最小值宏
#ifndef EZUI_MIN
#define EZUI_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//VS2010不支持enum class, 使用包装结构模拟作用域枚举, 保持 Xxx::Value 用法.
#ifdef EZUI_COMPILER_VS2010
#define EZUI_SCOPED_ENUM_FWD(ENUM_TYPE, BASE_TYPE) struct ENUM_TYPE;
#define EZUI_SCOPED_ENUM_BEGIN(ENUM_TYPE, BASE_TYPE) \
struct ENUM_TYPE {                                    \
	typedef BASE_TYPE enum_base_type;                 \
	enum enum_type
#define EZUI_SCOPED_ENUM_END(ENUM_TYPE)              \
	;                                                 \
	enum_base_type value;                             \
	ENUM_TYPE() : value(0) {}                         \
	ENUM_TYPE(enum_type v) : value(static_cast<enum_base_type>(v)) {} \
	explicit ENUM_TYPE(enum_base_type v) : value(v) {} \
	operator enum_base_type() const { return value; } \
};
#else
#define EZUI_SCOPED_ENUM_FWD(ENUM_TYPE, BASE_TYPE) enum class ENUM_TYPE : BASE_TYPE;
#define EZUI_SCOPED_ENUM_BEGIN(ENUM_TYPE, BASE_TYPE) enum class ENUM_TYPE : BASE_TYPE
#define EZUI_SCOPED_ENUM_END(ENUM_TYPE) ;
#endif

//生成枚举类常用操作符
#ifdef EZUI_COMPILER_VS2010
#define EZUI_ENUM_OPERATORS(ENUM_TYPE, BASE_TYPE)         \
inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) | static_cast<BASE_TYPE>(b)); \
}                                                           \
inline ENUM_TYPE operator|(ENUM_TYPE::enum_type a, ENUM_TYPE b) { \
    return ENUM_TYPE(a) | b;                                \
}                                                           \
inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE::enum_type b) { \
    return a | ENUM_TYPE(b);                                \
}                                                           \
inline ENUM_TYPE operator|(ENUM_TYPE::enum_type a, ENUM_TYPE::enum_type b) { \
    return ENUM_TYPE(a) | ENUM_TYPE(b);                     \
}                                                           \
inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) & static_cast<BASE_TYPE>(b)); \
}                                                            \
inline ENUM_TYPE operator&(ENUM_TYPE::enum_type a, ENUM_TYPE b) { \
    return ENUM_TYPE(a) & b;                                \
}                                                           \
inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE::enum_type b) { \
    return a & ENUM_TYPE(b);                                \
}                                                           \
inline ENUM_TYPE operator&(ENUM_TYPE::enum_type a, ENUM_TYPE::enum_type b) { \
    return ENUM_TYPE(a) & ENUM_TYPE(b);                     \
}                                                           \
inline ENUM_TYPE operator~(ENUM_TYPE a) {                        \
    return static_cast<ENUM_TYPE>(~static_cast<BASE_TYPE>(a));   \
}                                                              \
inline ENUM_TYPE operator~(ENUM_TYPE::enum_type a) {             \
    return ~ENUM_TYPE(a);                                      \
}                                                              \
inline ENUM_TYPE operator^(ENUM_TYPE a, ENUM_TYPE b) {           \
    return static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(a) ^ static_cast<BASE_TYPE>(b)); \
}                                                               \
inline ENUM_TYPE operator^(ENUM_TYPE::enum_type a, ENUM_TYPE b) { \
    return ENUM_TYPE(a) ^ b;                                    \
}                                                               \
inline ENUM_TYPE operator^(ENUM_TYPE a, ENUM_TYPE::enum_type b) { \
    return a ^ ENUM_TYPE(b);                                    \
}                                                               \
inline ENUM_TYPE operator^(ENUM_TYPE::enum_type a, ENUM_TYPE::enum_type b) { \
    return ENUM_TYPE(a) ^ ENUM_TYPE(b);                         \
}                                                               \
inline ENUM_TYPE& operator|=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a | b;                                                   \
    return a;                                                    \
}                                                              \
inline ENUM_TYPE& operator|=(ENUM_TYPE& a, ENUM_TYPE::enum_type b) { \
    a = a | b;                                                   \
    return a;                                                    \
}                                                              \
inline ENUM_TYPE& operator&=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a & b;                                                   \
    return a;                                                    \
}                                                               \
inline ENUM_TYPE& operator&=(ENUM_TYPE& a, ENUM_TYPE::enum_type b) { \
    a = a & b;                                                   \
    return a;                                                    \
}                                                               \
inline ENUM_TYPE& operator^=(ENUM_TYPE& a, ENUM_TYPE b) {        \
    a = a ^ b;                                                   \
    return a;                                                    \
}                                                                 \
inline ENUM_TYPE& operator^=(ENUM_TYPE& a, ENUM_TYPE::enum_type b) { \
    a = a ^ b;                                                   \
    return a;                                                    \
}                                                                 \
inline bool EnumExist(ENUM_TYPE flags, ENUM_TYPE flag) {          \
    return (static_cast<BASE_TYPE>(flags) & static_cast<BASE_TYPE>(flag)) != 0; \
}                                                                       \
inline bool EnumExist(ENUM_TYPE flags, ENUM_TYPE::enum_type flag) {     \
    return EnumExist(flags, ENUM_TYPE(flag));                           \
}                                                                       \
inline ENUM_TYPE& EnumAdd(ENUM_TYPE& flags, ENUM_TYPE flag) {\
    flags = static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(flags) | static_cast<BASE_TYPE>(flag)); \
    return flags;                                                        \
}                                                                        \
inline ENUM_TYPE& EnumAdd(ENUM_TYPE& flags, ENUM_TYPE::enum_type flag) { \
    return EnumAdd(flags, ENUM_TYPE(flag));                              \
}                                                                        \
inline ENUM_TYPE& EnumRemove(ENUM_TYPE& flags, ENUM_TYPE flag) {\
    flags = static_cast<ENUM_TYPE>(static_cast<BASE_TYPE>(flags) & ~static_cast<BASE_TYPE>(flag)); \
    return flags;                                                        \
}                                                                        \
inline ENUM_TYPE& EnumRemove(ENUM_TYPE& flags, ENUM_TYPE::enum_type flag) { \
    return EnumRemove(flags, ENUM_TYPE(flag));                           \
}
#else
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
#endif
