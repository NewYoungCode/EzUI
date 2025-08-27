/*/
Author:yang
Email:19980103ly@gmail.com/718987717@qq.com
*/

#pragma once

#include "UIDef.h"
#include "UIString.h"
#include "Resource.h"
#include "RenderTypes.h"
#include "Direct2DRender.h"

#undef LoadCursor
#undef LoadIcon

namespace ezui {
	struct MonitorInfo;
	class Object;
	class EventArgs;
	class ControlStyle;
	class IFrame;
	class Control;
	class Window;
	class Spacer;
	class ScrollBar;
	class Bitmap;
	enum class Cursor :ULONG_PTR;

#if 1
	typedef std::vector<Control*> Controls;
#else
	class UI_EXPORT Controls :public std::list<Control*> {
	public:
		//不要频繁使用此函数
		inline	Control* operator[](size_t right_pos)const
		{
			size_t pos = 0;
			for (auto& it : *this) {
				if (pos == right_pos) {
					return it;
				}
				++pos;
			}
			return NULL;
		}
	};
#endif

	//全局资源句柄
	extern UI_VAR_EXPORT HMODULE __EzUI__HINSTANCE;//全局实例
	extern UI_VAR_EXPORT Resource* __EzUI__Resource;//文件中的全局资源句柄
	extern UI_VAR_EXPORT DWORD __EzUI__ThreadId;//UI的线程Id
	extern UI_VAR_EXPORT HWND __EzUI_MessageWnd;//用于UI通讯的隐形窗口
	extern UI_VAR_EXPORT const std::list<ezui::MonitorInfo> __EzUI__MonitorInfos;//所有监视器信息

	//判断float是否为0(接近0)
	extern UI_EXPORT bool IsFloatZero(float num);
	//判断两个float是相等(两数是否接近)
	extern UI_EXPORT bool IsFloatEqual(float num1, float num2);
	//加载HICON
	extern UI_EXPORT HICON LoadIcon(const UIString& fileName);
	//装载字体
	extern UI_EXPORT void InstallFont(const UIString& fontFileName);
	//卸载字体
	extern UI_EXPORT void UnstallFont(const UIString& fontFileName);
	//复制内容到剪切板
	extern UI_EXPORT bool CopyToClipboard(int uFormat, void* pData, size_t size, HWND hWnd = NULL);
	//打开剪切板
	extern UI_EXPORT bool GetClipboardData(int uFormat, std::function<void(void*, size_t)> Callback, HWND hWnd = NULL);
	//复制unicode文字
	extern UI_EXPORT bool CopyToClipboard(const std::wstring& str, HWND hWnd = NULL);
	//粘贴unicode文字
	extern UI_EXPORT bool GetClipboardData(std::wstring* outStr, HWND hWnd = NULL);
	//自动获取文件资源(本地文件/资源文件)
	extern UI_EXPORT bool GetResource(const UIString& fileName, std::string* outData);
	//获取当前所有监视器的信息
	extern UI_EXPORT size_t GetMonitor(std::list<MonitorInfo>* outMonitorInfo);
	//获取用户当前所在的显示器
	extern UI_EXPORT void GetMontior(MonitorInfo* outInfo, HWND hWnd = NULL);
	//使用窗口的矩形位置获取所在的显示器
	extern UI_EXPORT void GetMontior(MonitorInfo* outInfo, const Rect& rect);
	//加载光标
	extern UI_EXPORT HCURSOR LoadCursor(Cursor cursorType);
	//加载光标(//需要释放)
	extern UI_EXPORT HCURSOR LoadCursor(const UIString& fileName);
	//释放光标
	extern UI_EXPORT void FreeCursor(HCURSOR hCursor);
	//默认处理OnNotify函数(处理一些控件的基础行为)
	extern UI_EXPORT bool DefaultNotify(Control* sender, EventArgs& args);

	class UI_EXPORT Color :public ezui::__EzUI__Color {
	public:
		Color(const ezui::__EzUI__Color& copy) { this->BGRA = copy.GetValue(); }
		Color(const DWORD& rgba = 0) :ezui::__EzUI__Color(rgba) {}
		Color(BYTE r, BYTE g, BYTE b, BYTE a = 255) :ezui::__EzUI__Color(r, g, b, a) {}
	public:
		//构建一个Color
		static Color Make(const UIString& colorStr, bool* isGood = NULL);
		virtual ~Color() {}
	};

#if USED_DIRECT2D
	class UI_EXPORT Image :public DXImage {
	private:
#ifdef _DEBUG
		UIString m_path;
#endif
	public:
		virtual ~Image() {}
		//创建带预乘Alpha的BGRA图片
		Image(int width, int height) :DXImage(width, height) {}
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(Bitmap* bitmap);
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const std::wstring& fileName) :DXImage(fileName) {}
		Image(const void* data, size_t dataCount) :DXImage(data, dataCount) {}
	public:
		//从资源或者本地文件自动构建一个Image
		static Image* Make(const UIString& fileOrRes);
	};
#endif
	// 定义用于保存显示器信息的结构体
	struct MonitorInfo {
		HMONITOR Monitor = NULL;
		//显示器的位置 多显示器下Y轴可能出现负数或者大于0的时候代表显示器在设置里面显示器是错位的(多显示器没有平行);
		//逻辑宽高
		ezui::Rect Rect;
		//工作区域
		ezui::Rect WorkRect;
		//显示器物理宽高
		Size Physical;
		//显示器缩放比例 1.0 1.25 1.5 1.75 2.0
		float Scale = 1.0f;
		//显示器帧率
		float FPS = 60;
		//是否为主显示器
		bool Primary = false;
	};
	struct WindowData {
		//缩放率
		float Scale = 1.0f;
		//单次绘图数量
		int PaintCount = 0;
#ifdef _DEBUG
		//是否开启debug模式
		bool Debug = false;
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
		//通知函数
		std::function<bool(Control*, EventArgs&)> SendNotify = NULL;//
		//清空控件标记等等...
		std::function<void(Control*)> CleanControl = NULL;
		//内部移动窗口的函数
		std::function<void()> MoveWindow = NULL;
		//内部使用标题部分移动窗口的函数
		std::function<void()> TitleMoveWindow = NULL;
		//处理消息过程的回调函数
		std::function<LRESULT(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> WndProc = NULL;

#ifdef _DEBUG
		virtual ~WindowData() {
		}
#endif

	};

	enum class LayoutState :byte {
		//无状态 (无需布局)
		None = 1,
		//挂起中
		Pend = 2,
		//布局中
		Layouting = 4
	};
	enum Event :long long {
		None = 1,
		OnMouseWheel = 2,
		OnMouseEnter = 4,
		OnMouseMove = 8,
		OnMouseLeave = 16,
		OnMouseDoubleClick = 32,
		OnMouseDown = 64,
		OnMouseUp = 128,
		OnKeyDown = 256,
		OnKeyUp = 512,
		OnPaint = 1024,
		OnFocus = 2048,
		OnKillFocus = 4096,
		OnKeyChar = 8192,
		OnMove = 16384,
		OnSize = 32768,
		OnRect = 65536,
		OnDpiChange = 131072,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnMouseDrag = OnMouseDown | OnMouseMove,
		OnMouseEvent = OnMouseWheel | OnMouseEnter | OnMouseMove | OnMouseLeave | OnMouseDoubleClick | OnMouseDown | OnMouseUp,
		OnKeyBoardEvent = OnKeyDown | OnKeyUp | OnKeyChar
	};
	//重载枚举的 | 运算符
	inline Event operator|(Event left, Event right)
	{
		return static_cast<Event>(static_cast<long long>(left) | static_cast<long long>(right));
	}
	//控件行为
	enum class ControlAction {
		None,
		Title,//具有移动窗口 双击最大化窗口的行为
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};
	enum class ControlState :int {
		None = 1,//无状态 则是使用_nowStyle缓存样式
		Static = 2,//静态
		Disabled = 4,//禁用状态
		Checked = 8,//选中状态
		Hover = 16,//鼠标悬浮
		Active = 32//鼠标按住
	};
	inline ControlState operator|(ControlState left, ControlState right)
	{
		return (ControlState)((int)left | (int)right);
	}
	inline ControlState operator&(ControlState a, ControlState b) {
		return (ControlState)((int)a & (int)b);
	}
	inline ControlState operator~(ControlState a) {
		return (ControlState)(~(int)a);
	}
	inline ControlState& operator|=(ControlState& left, ControlState right) {
		left = left | right;
		return left;
	}
	inline ControlState& operator&=(ControlState& left, ControlState right) {
		left = left & right;
		return left;
	}



	enum class DockStyle {
		// 摘要:
		//未设置
		None,
		// 摘要:
		//在父控件中 左右保持
		Horizontal,
		// 摘要:
		//在父控件中 上下保持
		Vertical,
		// 摘要:
		// 铺满整个父控件
		Fill
	};
	enum class MouseButton {
		// 摘要: 
		  //     未曾按下鼠标按钮。
		None,
		//
		// 摘要: 
		//     鼠标左按钮曾按下。
		Left,
		//
		// 摘要: 
		//     鼠标右按钮曾按下。
		Right,
		//
		// 摘要: 
		//     鼠标中按钮曾按下。
		Middle,
		//
		// 摘要: 
		//     第 1 个 XButton 曾按下。
		XButton1,
		//
		// 摘要: 
		//     第 2 个 XButton 曾按下。
		XButton2
	};
	enum class Cursor :ULONG_PTR
	{
		None = 0,//未指定
		APPSTARTING = (ULONG_PTR)IDC_APPSTARTING,// 标准的箭头和小沙漏
		ARROW = (ULONG_PTR)IDC_ARROW,// 标准的箭头
		CROSS = (ULONG_PTR)IDC_CROSS,// 十字光标
		HAND = (ULONG_PTR)IDC_HAND,// Windows 98/Me, Windows 2000/XP: Hand
		HELP = (ULONG_PTR)IDC_HELP,// 标准的箭头和问号
		IBEAM = (ULONG_PTR)IDC_IBEAM,// 工字光标
		ICON = (ULONG_PTR)IDC_ICON,// Obsolete for applications marked version 4.0 or later.
		NO = (ULONG_PTR)IDC_NO,// 禁止圈
		SIZE = (ULONG_PTR)IDC_SIZE,// Obsolete for applications marked version 4.0 or later. Use SIZEALL.
		SIZEALL = (ULONG_PTR)IDC_SIZEALL,// 四向箭头指向东、西、南、北
		SIZENESW = (ULONG_PTR)IDC_SIZENESW,// 双箭头指向东北和西南
		SIZENS = (ULONG_PTR)IDC_SIZENS, // 双箭头指向南北
		SIZENWSE = (ULONG_PTR)IDC_SIZENWSE,// 双箭头指向西北和东南
		SIZEWE = (ULONG_PTR)IDC_SIZEWE,// 双箭头指向东西
		UPARROW = (ULONG_PTR)IDC_UPARROW,// 垂直箭头
		WAIT = (ULONG_PTR)IDC_WAIT// 沙漏，Windows7下会显示为选择的圆圈表示等待
	};

	//基础事件
	class UI_EXPORT EventArgs {
	public:
		Event EventType = Event::None;
		EventArgs(Event eventType) {
			this->EventType = eventType;
		}
		virtual ~EventArgs() {};
	};
	//为鼠标事件提供基础数据
	class UI_EXPORT MouseEventArgs :public EventArgs {
	public:
		MouseButton Button = MouseButton::None;
		int ZDelta = 0;//方向
		Point Location;
	public:
		MouseEventArgs(Event eventType, const Point& location = Point(0, 0), MouseButton mouseButton = MouseButton::None, int ZDelta = 0) :EventArgs(eventType) {
			this->Button = mouseButton;
			this->Location = location;
			this->ZDelta = ZDelta;
		}
		virtual ~MouseEventArgs() {}
	};
	// 摘要: 
	//为键盘事件提供基础数据
	class UI_EXPORT KeyboardEventArgs :public EventArgs {
	public:
		/// <summary>
		/// 一般是指 键盘的ascii值
		/// </summary>
		WPARAM wParam;
		LPARAM lParam;
		KeyboardEventArgs(Event eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType) {
			this->wParam = wParam;
			this->lParam = lParam;
		}
		virtual ~KeyboardEventArgs() {}
	};
	//获取焦点
	class UI_EXPORT FocusEventArgs :public EventArgs {
	public:
		Control* Control;
		FocusEventArgs(ezui::Control* ctl) :EventArgs(Event::OnFocus) {
			this->Control = ctl;
		}
		virtual ~FocusEventArgs() {}
	};
	//失去焦点
	class UI_EXPORT KillFocusEventArgs :public EventArgs {
	public:
		Control* Control;
		KillFocusEventArgs(ezui::Control* ctl) :EventArgs(Event::OnKillFocus) {
			this->Control = ctl;
		}
		virtual ~KillFocusEventArgs() {}
	};
	//坐标发生改变
	class UI_EXPORT MoveEventArgs :public EventArgs {
	public:
		const ezui::Point& Location;
		MoveEventArgs(const ezui::Point& location) :EventArgs(Event::OnMove), Location(location) {}
		virtual ~MoveEventArgs() {}
	};
	//大小发生改变
	class UI_EXPORT SizeEventArgs :public EventArgs {
	public:
		const ezui::Size& Size;
		SizeEventArgs(const ezui::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//dpi发生变化
	class UI_EXPORT DpiChangeEventArgs :public EventArgs {
	public:
		float Scale = 1;
		DpiChangeEventArgs(float scale) :EventArgs(Event::OnDpiChange), Scale(scale) {}
		virtual ~DpiChangeEventArgs() {}
	};
	// 为 OnPaint 事件提供数据。
	class UI_EXPORT PaintEventArgs :public EventArgs {
	private:
		std::list<bool> m_layers;
		std::list<Point> m_offsets;
	public:
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		::HWND HWND = NULL;
		HDC DC = NULL;
		ezui::DXRender& Graphics;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		PaintEventArgs(ezui::DXRender& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
		//添加裁剪(速度较快)
		void PushLayer(const Rect& rectBounds);
		//添加异形裁剪 比较耗性能,但是可以异形抗锯齿裁剪
		void PushLayer(const Geometry& dxGeometry);
		//弹出最后一个裁剪
		void PopLayer();
		//放入一个偏移
		void PushOffset(const Point& offset);
		//弹出最后一个偏移
		void PopOffset();
	};
	// 为控件样式提供数据。
	class UI_EXPORT ControlStyle {
	public:
		//边框信息
		ezui::Border Border;
		//整体不透明度
		//UI_Float Opacity;
		//背景颜色
		Color BackColor = 0;
		//背景图片 如果指定的图片被删除 请必须将此置零
		Image* BackImage = NULL;
		//前景图片 如果指定的图片被删除 请必须将此置零
		Image* ForeImage = NULL;
		//字体名称 具有继承性
		std::wstring FontFamily;
		//字体大小 具有继承性
		int FontSize = 0;
		//前景颜色  具有继承性
		Color ForeColor;
		//鼠标样式
		HCURSOR Cursor = NULL;
		//旋转范围 0~360
		float Angle = 0;
	private:
		void operator=(const ControlStyle& right) {} //禁止直接赋值 因为这样会导致 Color执行拷贝使得Color变得不合法的有效
		ControlStyle(const ControlStyle& right) {} //禁止拷贝 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void Scale(float scale);
	};

	//指针管理
	template <typename T>
	class PtrManager {
	private:
		std::vector<T> m_ptrs;
	public:
		PtrManager() {}
		virtual ~PtrManager() {
			for (auto& obj : m_ptrs) {
				delete obj;
			}
		}
		void Add(const T& v) {
			if (v) {
				m_ptrs.push_back(v);
			}
		}
		void Remove(const T& v/*, bool bFree = false*/) {
			auto it = std::find(m_ptrs.begin(), m_ptrs.end(), v);
			if (it != m_ptrs.end()) {
				/*if (bFree) {
					delete(*it);
				}*/
				m_ptrs.erase(it);
			}
		}
		void Clear() {
			for (auto& obj : m_ptrs) {
				delete obj;
			}
			m_ptrs.clear();
		}
	};

	//常用对象基类
	class UI_EXPORT Object {
	private:
		//属性集合
		std::map<UIString, UIString> m_attrs;
		// 管理子对象的释放
		PtrManager<Object*> m_childObjects;
	public:
		//用户自定义数据
		UINT_PTR Tag = NULL;
	public:
		Object(Object* parentObject = NULL);
		virtual ~Object();
	public:
		//设置属性
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue);
		//获取属性
		virtual UIString GetAttribute(const UIString& attrName);
		//获取全部属性
		virtual const std::map<UIString, UIString>& GetAttributes();
		//移除某个属性
		virtual void RemoveAttribute(const UIString& attrName);
		//绑定对象(跟随释放)
		virtual Object* Attach(Object* obj);
		//分离对象(解除跟随释放)
		virtual void Detach(Object* obj);
	};

	//原理采用PostMessage
	template<class Func, class... Args>
	bool BeginInvoke(Func&& f, Args&& ...args) {
		HWND hWnd = ezui::__EzUI_MessageWnd;
		if (hWnd == NULL || !::IsWindow(hWnd)) {
			return false;
		}
		std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (::PostMessage(hWnd, WM_GUI_SYSTEM, WM_GUI_BEGININVOKE, (LPARAM)func) == LRESULT(0)) {
			delete func;
			return false;
		}
		return true;
	}
	//原理采用SendMessage
	template<class Func, class... Args>
	bool Invoke(Func&& f, Args&& ...args) {
		std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (::GetCurrentThreadId() == ezui::__EzUI__ThreadId) {
			func();
			return true;
		}
		HWND hWnd = ezui::__EzUI_MessageWnd;
		if (hWnd == NULL || !::IsWindow(hWnd)) {
			return false;
		}
		if (::SendMessage(hWnd, WM_GUI_SYSTEM, WM_GUI_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
			return false;
		}
		return true;
	}

};
