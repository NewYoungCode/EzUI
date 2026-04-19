#pragma once
#include "UIDef.h"

#include "utils/Resource.h"
#include "utils/String.h"
#include "utils/Collection.h"
#include "utils/SmartPtr.h"
#include "utils/PropertyValue.h"
#include "utils/Utility.h"

#include "graphics/GraphicsTypes.h"
#include "graphics/Graphics.h"
#include "graphics/Image.h"

#undef LoadCursor
#undef LoadIcon

namespace ezui {
	class Object;
	enum class Cursor : ULONG_PTR;
	struct StyleRule;
	class UIStyle;
	struct DisplayInfo;
	class EventArgs;
	class ControlStyle;
	class Bitmap;
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
	enum class Cursor : ULONG_PTR
	{
		None = 0,                       // 未指定光标
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
	};
	/*============================================================枚举============================================================*/


	namespace detail {
		//全局资源句柄(内部使用)
		struct GlobalContext {
			HMODULE EzUI_HINSTANCE = NULL;//全局实例
			Resource* EzUI_Resource = NULL;//文件中的全局资源句柄
			WindowHandle EzUI_MessageWnd = NULL;//用于UI通讯的隐形窗口
			std::thread::id EzUI_ThreadId;//UI的线程Id
			ToolTip* EzUI_ToolTip = NULL;//全局提示控件
			Collection<WindowHandle> EzUI_g_hWnds;//存储所有使用本框架产生的窗口句柄
			void AddWindow(WindowHandle handle);
			void RemoveWindow(WindowHandle handle);
			void DestroyAllWindows();
		};

		extern EZUI_API int AddObjectCount();
		extern EZUI_API int SubtractObjectCount();
		extern EZUI_API int GetObjectCount();

		//获取全局上下文(内部使用)
		extern EZUI_API GlobalContext* GetGlobalContext();
		//设置全局上下文(内部使用)
		extern EZUI_API void SetGlobalContext(GlobalContext*);
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
		bool Focused = false;
#ifdef DEBUG
		//是否调试布局(显示内/外边距/高亮边框)
		bool DebugLayout = false;
		//调试模式下的特有字段
		int ColorIndex = 0;
		Color DebugColor;
		std::vector<Color> DebugColors{ Color::Red,Color::Green,Color::Blue,Color::Black,Color::White };
#endif
		//主窗类的实例
		ezui::Window* Window = NULL;
		//使一个区域无效
		std::function<void(const Rect&)> InvalidateRect = NULL;
		//立即更新全部无效区域
		std::function<void()> Refresh = NULL;
		//内部移动窗口的函数
		std::function<void()> MoveWindow = NULL;
		//内部使用标题部分移动窗口的函数
		std::function<void()> TitleMoveWindow = NULL;
		//处理消息过程的回调函数
		std::function<LRESULT(WindowHandle hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> WndProc = NULL;
		//设置焦点控件
		std::function<void(Control*)> SetFocus = NULL;
		//为当前具有焦点的框架插入文字
		std::function<void(const UIString&)> InsertText = NULL;
	};


	//常用对象基类
	class EZUI_API Object {
	private:
		//对象是否存活的标志(智能指针包装的原子布尔值)
		std::shared_ptr<std::atomic<bool>>* m_alive = NULL;
		//属性集合
		std::map<UIString, UIString>* m_attrs = NULL;
		//用户自定义数据
		std::map<int, void*>* m_userDatas = NULL;
		// 管理子对象的释放
		PtrManager<Object*> m_childObjects;
#ifdef DEBUG
		Object* m_ownerObject = NULL; // 拥有者对象（如果有）
#endif // DEBUG
	private:
		Object(const Object&) = delete;            // 禁止拷贝构造
		Object& operator=(const Object&) = delete; // 禁止拷贝赋值
		Object(Object&&) = delete;                 // 禁止移动构造
		Object& operator=(Object&&) = delete;      // 禁止移动赋值
	public:
		// 如果传入 ownerObject，则该对象的生命周期由 ownerObject 管理
		// ownerObject 析构时会自动销毁当前对象
		// 若为 NULL，则该对象具有独立生命周期
		explicit Object(Object* ownerObject = NULL);
		virtual ~Object();
	public:
		//获取对象弱指针(用于安全引用对象指针，防止野指针)
		template<typename T = Object>
		WeakPtr<T> GetWeakPtr() {
			WeakPtr<T> ptr((T*)this, GetAlive());
			return ptr;
		}
		//获取对象是否存活的共享指针(用于查看对象生命周期)
		std::shared_ptr<std::atomic<bool>> GetAlive();
		//设置用户自定义数据
		virtual void SetUserData(void* userData, int dataIndex = 0);
		//获取用户自定义数据
		virtual void* GetUserData(int dataIndex = 0)const;
		//删除用户数据
		virtual bool EraseUserData(int dataIndex);
		//设置属性
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue);
		//获取属性
		virtual const UIString& GetAttribute(const UIString& attrName)const;
		//获取全部属性
		virtual const std::map<UIString, UIString>& GetAttributes();
		//移除某个属性
		virtual void RemoveAttribute(const UIString& attrName);
		//绑定对象(跟随释放)
		virtual Object* Attach(Object* obj);
		//分离对象(解除跟随释放)
		virtual void Detach(Object* obj);
		//延迟删除
		void DeleteLater();
		//尝试转为指定类型
		template<typename T>
		T* As() {
			T* out = dynamic_cast<T*>(this);
			return out;
		}
	};

	//向UI线程投递异步调用函数 (不会阻塞当前线程) 
	//原理是PostMessage到UI线程的消息队列中
	template<class Func, class... Args>
	bool BeginInvoke(Func&& f, Args&& ...args) {
		auto* ctt = detail::GetGlobalContext();
		if (!ctt)return false;
		WindowHandle hWnd = ctt->EzUI_MessageWnd;
		if (hWnd == NULL || !::IsWindow(hWnd)) {
			EZUI_INFO(L"BeginInvoke failed: Message window is not available.");
			return false;
		}
		std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (::PostMessage(hWnd, EZUI_WM_SYSTEM, EZUI_WM_BEGININVOKE, (LPARAM)func) == LRESULT(0)) {
			delete func;
			EZUI_INFO(L"BeginInvoke failed: PostMessage returned 0.");
			return false;
		}
		return true;
	}

	//向UI线程投递同步调用函数 (会阻塞当前线程直到函数执行完毕)
	//原理是SendMessage到UI线程的消息队列中
	template<class Func, class... Args>
	bool Invoke(Func&& f, Args&& ...args) {
		auto* ctt = detail::GetGlobalContext();
		if (!ctt)return false;
		std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (std::this_thread::get_id() == ctt->EzUI_ThreadId) {
			func();
			return true;
		}
		WindowHandle hWnd = ctt->EzUI_MessageWnd;
		if (hWnd == NULL || !::IsWindow(hWnd)) {
			EZUI_INFO(L"Invoke failed: Message window is not available.");
			return false;
		}
		if (::SendMessage(hWnd, EZUI_WM_SYSTEM, EZUI_WM_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
			EZUI_INFO(L"Invoke failed: SendMessage returned -1.");
			return false;
		}
		return true;
	}
};
