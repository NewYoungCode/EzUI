#pragma once
#include "UIDef.h"
#include "UIThreadDispatcher.h"

#ifdef EZUI_COMPILER_VS2010
#include <float.h>
#include "platform/window/mutex.h"
#include "platform/window/condition_variable.h"
#include "platform/window/thread.h"
#else
//c++11
#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
namespace ezui {
	using std::thread;
	using std::condition_variable;
	using std::mutex;
	using std::unique_ptr;
	using std::lock_guard;
	using std::unique_lock;
	using std::once_flag;
	namespace this_thread = std::this_thread;
};
#endif

#include "Utility.h"
#include "MessageQueue.h"
#include "Resource.h"
#include "String.h"
#include "Collection.h"
#include "SmartPtr.h"
#include "Object.h"
#include "PropertyValue.h"

#include "graphics/GraphicsTypes.h"
#include "graphics/Graphics.h"
#include "graphics/Image.h"

#undef LoadCursor
#undef LoadIcon

namespace ezui {
	EZUI_SCOPED_ENUM_FWD(Cursor, ULONG_PTR)
		struct StyleRule;
	class UIStyle;
	struct DisplayInfo;
	class EventArgs;
	class ControlStyle;
	class Bitmap;
	class IWindow;
	class Window;
	struct WindowBridge;

	class Control;
	class DataGridView;
	class Frame;
	class Spacer;
	class ScrollBar;
	class VScrollBar;
	class HScrollBar;
	class TabControl;
	class TextBox;
	class TileListView;
	class TreeView;
	class VLayout;
	class VListView;
	class Button;
	class CheckBox;
	class ComboBox;
	class HLayout;
	class HListView;
	class Label;
	class PagedListView;
	class PictureBox;
	class RadioButton;

	class BorderlessWindow;
	class ToolTip;

	namespace detail {
		class WindowRenderManager;
		class RenderContext;
	}

	// 光标类型（基于 Windows 系统光标资源映射）
	EZUI_SCOPED_ENUM_BEGIN(Cursor, ULONG_PTR)
	{
		None = 0,                       // 未指定光标
#ifdef EZUI_COMPILER_VS2010
			AppStarting = 32650,             // 箭头 + 小沙漏（启动中）
			Default = 32512,                 // 默认箭头光标
			Cross = 32515,                   // 十字准星光标
			Pointer = 32649,                 // 手型可点击光标
			Help = 32651,                    // 帮助光标（箭头 + 问号）
			Text = 32513,                    // 文本输入光标（工字形）
			NotAllowed = 32648,              // 禁止操作光标
			UpArrow = 32516,                 // 上箭头光标
			Wait = 32514,                    // 等待光标（沙漏）

			ResizeAll = 32646,               // 四向调整光标
			Move = 32646,                    // 四向调整光标（ResizeAll 的别名，兼容 CSS move）

			// 以下单方向值为 CSS 兼容别名，Windows 不区分单方向
			ResizeN = 32645,                 // ↑ 垂直调整（n-resize）
			ResizeS = 32645,                 // ↓ 垂直调整（s-resize）
			ResizeE = 32644,                 // → 水平调整（e-resize）
			ResizeW = 32644,                 // ← 水平调整（w-resize）

			ResizeNS = 32645,                // ↕ 垂直调整（ns-resize）
			ResizeWE = 32644,                // ↔ 水平调整（we-resize）
			ResizeNE = 32643,                // ↗ 对角调整（ne-resize）
			ResizeSW = 32643,                // ↙ 对角调整（sw-resize）
			ResizeNW = 32642,                // ↖ 对角调整（nw-resize）
			ResizeSE = 32642                 // ↘ 对角调整（se-resize）
#else
			AppStarting = (ULONG_PTR)IDC_APPSTARTING, // 箭头 + 小沙漏（启动中）
			Default = (ULONG_PTR)IDC_ARROW,     // 默认箭头光标
			Cross = (ULONG_PTR)IDC_CROSS,     // 十字准星光标
			Pointer = (ULONG_PTR)IDC_HAND,       // 手型可点击光标
			Help = (ULONG_PTR)IDC_HELP,       // 帮助光标（箭头 + 问号）
			Text = (ULONG_PTR)IDC_IBEAM,      // 文本输入光标（工字形）
			NotAllowed = (ULONG_PTR)IDC_NO,   // 禁止操作光标
			UpArrow = (ULONG_PTR)IDC_UPARROW,   // 上箭头光标
			Wait = (ULONG_PTR)IDC_WAIT,          // 等待光标（沙漏）

			ResizeAll = (ULONG_PTR)IDC_SIZEALL, // 四向调整光标
			Move = (ULONG_PTR)IDC_SIZEALL,      // 四向调整光标（ResizeAll 的别名，兼容 CSS move）

			// 以下单方向值为 CSS 兼容别名，Windows 不区分单方向
			ResizeN = (ULONG_PTR)IDC_SIZENS,      // ↑ 垂直调整（n-resize）
			ResizeS = (ULONG_PTR)IDC_SIZENS,      // ↓ 垂直调整（s-resize）
			ResizeE = (ULONG_PTR)IDC_SIZEWE,      // → 水平调整（e-resize）
			ResizeW = (ULONG_PTR)IDC_SIZEWE,      // ← 水平调整（w-resize）

			ResizeNS = (ULONG_PTR)IDC_SIZENS,      // ↕ 垂直调整（ns-resize）
			ResizeWE = (ULONG_PTR)IDC_SIZEWE,      // ↔ 水平调整（we-resize）
			ResizeNE = (ULONG_PTR)IDC_SIZENESW,   // ↗ 对角调整（ne-resize）
			ResizeSW = (ULONG_PTR)IDC_SIZENESW,   // ↙ 对角调整（sw-resize）
			ResizeNW = (ULONG_PTR)IDC_SIZENWSE,   // ↖ 对角调整（nw-resize）
			ResizeSE = (ULONG_PTR)IDC_SIZENWSE    // ↘ 对角调整（se-resize）
#endif
	} EZUI_SCOPED_ENUM_END(Cursor)
			/*============================================================枚举============================================================*/

			namespace detail {
			//全局资源句柄(框架内部使用)
			struct GlobalContext {
				HMODULE EzUI_HINSTANCE;//全局实例
				Resource* EzUI_Resource;//文件中的全局资源句柄
				WindowHandle EzUI_MessageWnd;//用于UI通讯的隐形窗口
				thread::id EzUI_ThreadId;//UI的线程Id
				ToolTip* EzUI_ToolTip;//全局提示控件
				Collection<WindowHandle> EzUI_WindowHandles;//存储所有使用本框架产生的窗口句柄
				std::wstring EzUI_WindowClass;
				std::wstring EzUI_InvokeClass;
				std::wstring EzUI_TrayIconClass;
				GlobalContext()
					: EzUI_HINSTANCE(NULL), EzUI_Resource(NULL), EzUI_MessageWnd(NULL),
					EzUI_ToolTip(NULL) {
				}
				void AddWindow(WindowHandle handle);
				void RemoveWindow(WindowHandle handle);
				void DestroyAllWindows();
				bool IsUIThread() {
					return (this_thread::get_id() == EzUI_ThreadId);
				}
				virtual ~GlobalContext() {
					EZUI_INFO(L"Global context shutdown.");
				}
			};

			//获取全局上下文(框架内部使用)
			extern EZUI_API std::weak_ptr<GlobalContext> GetGlobalContext();
			//设置全局上下文(框架内部使用)
			extern EZUI_API void SetGlobalContext(std::weak_ptr<GlobalContext>);
		};

		//获取WindowBridge
		extern EZUI_API WindowBridge* GetWindowBridge(WindowHandle handle);
		//是否是ezui创建的窗口
		extern EZUI_API bool IsEzUIWindow(WindowHandle handle);
		//设置全局提示控件(需要手动释放)
		extern EZUI_API void SetGlobalToolTip(ToolTip* toolTip);
		//获取全局提示控件
		extern EZUI_API ToolTip* GetGlobalToolTip();
		//自动获取文件资源(本地文件/资源文件)
		extern EZUI_API bool GetResource(const UIString& fileName, std::string* outData);
		//返回全局资源管理器实例
		extern EZUI_API Resource* GetResourceManager();
		//加载光标
		extern EZUI_API HCURSOR LoadCursor(Cursor cursorType);
		//加载光标(//需要释放)
		extern EZUI_API HCURSOR LoadCursor(const UIString& fileName);
		//释放光标
		extern EZUI_API void FreeCursor(HCURSOR hCursor);
		//加载HICON
		extern EZUI_API HICON LoadIcon(const UIString& fileName);
		//装载字体到框架中(从文件或者资源文件中加载字体)
		extern EZUI_API bool InstallFont(const UIString& fontFileName, UIString* outFontName = NULL);
		//装载字体到框架中(从内存中加载字体)
		extern EZUI_API bool InstallFont(const char* data, uint32_t size, UIString* outFontName = NULL);

		namespace detail {
			//获取窗口基于屏幕的位置
			extern EZUI_API void GetWindowRect(WindowHandle hWnd, Rect* outRect);
			//获取窗口客户区
			extern EZUI_API void GetClientRect(WindowHandle hWnd, Rect* outRect);
		}

		// 窗口桥接层（Control 与 Window 之间的通信接口）
		// 提供窗口能力调用与部分运行状态访问
		struct WindowBridge {
			//是否具有焦点
			bool Focused;
#ifdef DEBUG
			//是否调试布局(显示内/外边距/高亮边框)
			bool DebugLayout;
			//调试模式下的特有字段
			int ColorIndex;
			Color DebugColor;
			std::vector<Color> DebugColors;
#endif
			//主窗类的实例
			ezui::Window* Window;
			//使一个区域无效
			std::function<void(const Rect&)> InvalidateRect;
			//立即更新全部无效区域
			std::function<void()> Refresh;
			//内部移动窗口的函数
			std::function<void()> MoveWindow;
			//内部使用标题部分移动窗口的函数
			std::function<void()> TitleMoveWindow;
			//处理消息过程的回调函数
			std::function<LRESULT(WindowHandle hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> WndProc;
			//设置焦点控件
			std::function<void(Control*)> SetFocus;
			//为当前具有焦点的框架插入文字
			std::function<void(const UIString&)> InsertText;
			WindowBridge()
				: Focused(false)
#ifdef DEBUG
				, DebugLayout(false), ColorIndex(0)
#endif
				, Window(NULL), InvalidateRect(NULL), Refresh(NULL), MoveWindow(NULL), TitleMoveWindow(NULL),
				WndProc(NULL), SetFocus(NULL), InsertText(NULL) {
#ifdef DEBUG
				DebugColors.push_back(Color::Red);
				DebugColors.push_back(Color::Green);
				DebugColors.push_back(Color::Blue);
				DebugColors.push_back(Color::Black);
				DebugColors.push_back(Color::White);
#endif
			}
		};


#ifdef EZUI_COMPILER_VS2010
		//向UI线程投递异步调用函数 (不会阻塞当前线程)
		//原理是PostMessage到UI线程的消息队列中
		template<class Func>
		bool BeginInvoke(Func&& f) {
			std::function<void()>* func = new std::function<void()>(std::forward<Func>(f));
			if (!detail::PostUIThreadTask(func)) {
				delete func;
				EZUI_INFO(L"BeginInvoke failed: Dispatcher is not available.");
				return false;
			}
			return true;
		}
		//向UI线程投递同步调用函数 (会阻塞当前线程直到函数执行完毕)
		//原理是SendMessage到UI线程的消息队列中
		template<class Func>
		bool Invoke(Func&& f) {
			WindowHandle hWnd = NULL;
			bool isUIThread = false;
			if (auto ctx = detail::GetGlobalContext().lock()) {
				hWnd = ctx->EzUI_MessageWnd;
				isUIThread = ctx->IsUIThread();
			}
			if (!::IsWindow(hWnd)) {
				EZUI_INFO(L"Invoke failed: Message window is not available.");
				return false;
			}
			std::function<void()> func(std::forward<Func>(f));
			if (isUIThread) {
				func();
				return true;
			}
			if (::SendMessage(hWnd, EZUI_WM_SYSTEM, EZUI_WM_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
				EZUI_INFO(L"Invoke failed: SendMessage returned -1.");
				return false;
			}
			return true;
		}
#else
		//向UI线程投递异步调用函数 (不会阻塞当前线程)
		//原理是PostMessage到UI线程的消息队列中
		template<class Func, class... Args>
		bool BeginInvoke(Func&& f, Args&& ...args) {
			std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			if (!detail::PostUIThreadTask(func)) {
				delete func;
				EZUI_INFO(L"BeginInvoke failed: Dispatcher is not available.");
				return false;
			}
			return true;
		}

		//向UI线程投递同步调用函数 (会阻塞当前线程直到函数执行完毕)
		//原理是SendMessage到UI线程的消息队列中
		template<class Func, class... Args>
		bool Invoke(Func&& f, Args&& ...args) {
			WindowHandle hWnd = NULL;
			bool isUIThread = false;
			if (auto ctx = detail::GetGlobalContext().lock()) {
				hWnd = ctx->EzUI_MessageWnd;
				isUIThread = ctx->IsUIThread();
			}
			if (!::IsWindow(hWnd)) {
				EZUI_INFO(L"Invoke failed: Message window is not available.");
				return false;
			}
			std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			if (isUIThread) {
				func();
				return true;
			}

			if (::SendMessage(hWnd, EZUI_WM_SYSTEM, EZUI_WM_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
				EZUI_INFO(L"Invoke failed: SendMessage returned -1.");
				return false;
			}
			return true;
		}
#endif

};
