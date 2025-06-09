/*/
Author:yang
Email:19980103ly@gmail.com/718987717@qq.com
*/

#pragma once
#include "UIDef.h"
#include "EString.h"
#include "Resource.h"
#include "RenderType.h"
#include "Direct2DRender.h"

namespace EzUI {
	struct MonitorInfo;
	class IControl;
	class EventArgs;
	class ControlStyle;
	class Control;
	class Window;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;
	typedef std::vector<Control*> Controls;

	//全局资源句柄
	extern UI_VAR_EXPORT WCHAR __EzUI__WindowClassName[];//窗口类名
	extern UI_VAR_EXPORT HMODULE __EzUI__HINSTANCE;//全局实例
	extern UI_VAR_EXPORT Resource* __EzUI__Resource;//文件中的全局资源句柄
	extern UI_VAR_EXPORT DWORD __EzUI__ThreadId;//UI的线程Id
	extern UI_VAR_EXPORT std::list<HWND> __EzUI__WNDS;//存储所有使用本框架产生的窗口句柄
	extern UI_VAR_EXPORT const std::list<EzUI::MonitorInfo> __EzUI__MonitorInfos;//所有监视器信息

	//装载字体
	extern UI_EXPORT void InstallFont(const EString& fontFileName);
	//卸载字体
	extern UI_EXPORT void UnstallFont(const EString& fontFileName);
	//复制内容到剪切板
	extern UI_EXPORT bool CopyToClipboard(int_t uFormat, void* pData, size_t size, HWND hWnd = NULL);
	//打开剪切板
	extern UI_EXPORT bool GetClipboardData(int_t uFormat, std::function<void(void*, size_t)> Callback, HWND hWnd = NULL);
	//复制unicode文字
	extern UI_EXPORT bool CopyToClipboard(const std::wstring& str, HWND hWnd = NULL);
	//粘贴unicode文字
	extern UI_EXPORT bool GetClipboardData(std::wstring* outStr, HWND hWnd = NULL);
	//自动获取文件资源(本地文件/资源文件)
	extern UI_EXPORT bool GetResource(const EString& fileName, std::string* outData);
	//获取当前所有监视器的信息
	extern UI_EXPORT size_t GetMonitor(std::list<MonitorInfo>* outMonitorInfo);
	//获取用户当前所在的显示器
	extern UI_EXPORT void GetMontior(MonitorInfo* outInfo, HWND hWnd = NULL);
	//使用窗口的矩形位置获取所在的显示器
	extern UI_EXPORT void GetMontior(MonitorInfo* outInfo, const Rect& rect);
	//加载光标
	extern UI_EXPORT HCURSOR LoadCursor(Cursor cursorType);
	//加载光标(//需要释放)
	extern UI_EXPORT HCURSOR LoadCursor(const EString& fileName);
	//释放光标
	extern UI_EXPORT void FreeCursor(HCURSOR hCursor);

	class UI_EXPORT Color :public EzUI::__EzUI__Color {
	public:
		Color(const EzUI::__EzUI__Color& copy) { this->RGBA = copy.GetValue(); }
		Color(const DWORD& rgba = 0) :EzUI::__EzUI__Color(rgba) {}
		Color(BYTE r, BYTE g, BYTE b, BYTE a = 255) :EzUI::__EzUI__Color(r, g, b, a) {}
	public:
		static Color Make(const EString& colorStr) {
			if (colorStr.find("#") == 0) { //"#4e6ef2"
				auto rStr = colorStr.substr(1, 2);
				auto gStr = colorStr.substr(3, 2);
				auto bStr = colorStr.substr(5, 2);
				DWORD r, g, b;
				sscanf_s(rStr.c_str(), "%x", &r);
				sscanf_s(gStr.c_str(), "%x", &g);
				sscanf_s(bStr.c_str(), "%x", &b);
				//Argb = MakeARGB(255, r, g, b);
				return Color((BYTE)r, (BYTE)g, (BYTE)b);
			}
			if (colorStr.find("rgb") == 0) { //"rgb(255,100,2,3)"
				size_t pos1 = colorStr.find("(");
				size_t pos2 = colorStr.rfind(")");
				EString rgbStr = colorStr.substr(pos1 + 1, pos2 - pos1 - 1);
				auto rgbList = rgbStr.split(",");
				BYTE r, g, b;
				r = std::stoi(rgbList.at(0));
				g = std::stoi(rgbList.at(1));
				b = std::stoi(rgbList.at(2));
				BYTE a = 255;
				//考虑到rgba
				if (rgbList.size() > 3) {
					std::string aStr = rgbList.at(3);
					if (aStr.find(".") != std::string::npos) {
						//浮点型0~1
						a = (BYTE)(255 * std::atof(aStr.c_str()) + 0.5);
					}
					else {
						//整数型0~255
						a = std::stoi(aStr.c_str());
					}
				}
				return Color(r, g, b, a);
			}
			return Color();
		}
		virtual ~Color() {}
	};

#if USED_DIRECT2D
	class UI_EXPORT Image :public DXImage {
	public:
		virtual ~Image() {}
		Image(int_t width, int_t height) :DXImage(width, height) {}
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const std::wstring& fileName) :DXImage(fileName) {}
		Image(const void* data, size_t dataCount) :DXImage(data, dataCount) {}
	public:
		//从资源或者本地文件自动构建一个Image
		static Image* Make(const EString& fileOrRes) {
			//本地文件中获取
			std::wstring wstr = fileOrRes.unicode();
			DWORD dwAttr = GetFileAttributesW(wstr.c_str());
			if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
				return new Image(wstr);
			}
			//从资源中获取
			if (EzUI::__EzUI__Resource) {
				std::string data;
				EzUI::__EzUI__Resource->GetFile(fileOrRes, &data);
				if (data.empty()) {
					return NULL;
				}
				return new Image(data.c_str(), data.size());
			}
			return NULL;
		}
	};
#endif
	// 定义用于保存显示器信息的结构体
	struct MonitorInfo {
		HMONITOR Monitor = NULL;
		//显示器的位置 多显示器下Y轴可能出现负数或者大于0的时候代表显示器在设置里面显示器是错位的(多显示器没有平行);
		EzUI::Rect Rect;//逻辑宽高
		EzUI::Rect WorkRect;//工作区域
		Size Physical;//显示器物理宽高
		float Scale = 1.0f;//显示器缩放比例
		float FPS = 60;//显示器帧率
		bool Primary = false;//是否为主显示器
	};
	struct WindowData {
		float Scale = 1.0f;//缩放率
		Control* FocusControl = NULL;//具有焦点的控件
		Control* InputControl = NULL;//输入框
		int_t PaintCount = 0;
#ifdef _DEBUG
		bool Debug = false;//是否开启debug模式
		int_t ColorIndex = 0;//调试模式下的特有字段
		Color DebugColor;
		std::vector<Color> DebugColors{ Color::Red,Color::Green,Color::Blue,Color::Black,Color::White };
#endif
		EzUI::Window* Window = NULL;//主窗类的实例
		HWND HANDLE = NULL;//窗口句柄
		std::function<void(const Rect&)> InvalidateRect = NULL;//使一个区域无效
		std::function<void()> UpdateWindow = NULL;//立即更新全部无效区域
		std::function<bool(Control*, EventArgs&)> Notify = NULL;//
		std::function<void(Control*)> RemoveControl = NULL;//清空控件标记等等...
		std::function<void(Control*, const std::wstring&)> SetTips = NULL;//设置tips文字
		std::function<void(Control*)> DelTips = NULL;//删除tips文字
	};

	class UI_EXPORT StopWatch {
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
			beg_t = end_t;
			return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		}
	};


	enum class LayoutState :byte {
		None = 1, //无状态 (无需布局)
		Pend = 2,//挂起中
		Layouting = 4//布局中
	};
	enum Event :long long {
		None = 1,
		OnMouseWheel = 2,
		OnMouseEnter = 4,
		OnMouseMove = 8,
		OnMouseLeave = 16,
		OnMouseClick = 32,
		OnMouseDoubleClick = 64,
		OnMouseDown = 128,
		OnMouseUp = 256,
		OnKeyDown = 512,
		OnKeyUp = 1024,
		OnPaint = 2048,
		OnFocus = 4096,
		OnKillFocus = 8192,
		OnKeyChar = 16384,
		OnLocation = 32768,
		OnSize = 65536,
		OnRect = 131072,
		OnDpiChange = 262144,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnMouseDrag = OnMouseDown | OnMouseMove,
		OnMouseEvent = OnMouseWheel | OnMouseEnter | OnMouseMove | OnMouseLeave | OnMouseClick | OnMouseDoubleClick | OnMouseDown | OnMouseUp,
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
	enum class ControlState {
		None,//无状态 则是使用_nowStyle缓存样式
		Static,//静态
		Checked,//选中状态
		Hover,//鼠标悬浮
		Active//鼠标按住
	};
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
		int_t ZDelta = 0;//方向
		Point Location;
	public:
		MouseEventArgs(Event eventType, const Point& location = Point(0, 0), MouseButton mouseButton = MouseButton::None, int_t ZDelta = 0) :EventArgs(eventType) {
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
		FocusEventArgs(EzUI::Control* ctl) :EventArgs(Event::OnFocus) {
			this->Control = ctl;
		}
		virtual ~FocusEventArgs() {}
	};
	//失去焦点
	class UI_EXPORT KillFocusEventArgs :public EventArgs {
	public:
		Control* Control;
		KillFocusEventArgs(EzUI::Control* ctl) :EventArgs(Event::OnKillFocus) {
			this->Control = ctl;
		}
		virtual ~KillFocusEventArgs() {}
	};
	//坐标发生改变
	class UI_EXPORT LocationEventArgs :public EventArgs {
	public:
		const EzUI::Point& Location;
		LocationEventArgs(const EzUI::Point& location) :EventArgs(Event::OnLocation), Location(location) {}
		virtual ~LocationEventArgs() {}
	};
	//大小发生改变
	class UI_EXPORT SizeEventArgs :public EventArgs {
	public:
		const EzUI::Size& Size;
		SizeEventArgs(const EzUI::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//矩形发生改变
	class UI_EXPORT RectEventArgs :public EventArgs {
	public:
		const EzUI::Rect& Rect;
		RectEventArgs(const EzUI::Rect& rect) :EventArgs(Event::OnRect), Rect(rect) {}
		virtual ~RectEventArgs() {}
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
		std::list<bool> layers;
		std::list<Point> offsets;
	public:
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::DXRender& Graphics;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		PaintEventArgs(EzUI::DXRender& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
		void PushLayer(const Rect& rectBounds);//速度较快
		void PushLayer(const Geometry& dxGeometry);//比较耗性能,但是可以异形抗锯齿裁剪
		void PopLayer();//弹出最后一个裁剪
		void PushOffset(const Point& offset);
		void PopOffset();//弹出最后一个偏移
	};
	// 为控件样式提供数据。
	class UI_EXPORT ControlStyle {
	public:
		EzUI::Border Border;//边框信息
		//UI_Float Opacity;//整体不透明度
		Color BackColor = 0;//背景颜色
		Image* BackImage = NULL;//背景图片 如果指定的图片被删除 请必须将此置零
		Image* ForeImage = NULL;//前景图片 如果指定的图片被删除 请必须将此置零
		std::wstring FontFamily;//字体名称 具有继承性
		int_t FontSize = 0;//字体大小 具有继承性 此处采用int
		Color ForeColor;//前景颜色  具有继承性
		HCURSOR Cursor = NULL;//鼠标样式
		float Angle = 0;//旋转范围 0~360
	private:
		void operator=(const ControlStyle& right) {} //禁止直接赋值 因为这样会导致 Color执行拷贝使得Color变得不合法的有效
		ControlStyle(const ControlStyle& right) {} //禁止拷贝 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void Scale(float scale);
		void SetStyleSheet(const EString& styleStr, const std::function<void(Image*)>& callback = NULL);
		void SetStyle(const EString& key, const EString& value, const std::function<void(Image*)>& callback = NULL);
	};

	class UI_EXPORT IControl {
	private:
		std::map<EString, EString> _attrs;
	public:
		const bool IsWindow = false;
		void* Tag = NULL;
		WindowData* PublicData = NULL;//公共数据
	public:
		IControl();
		virtual ~IControl();
	public:
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//设置属性
		virtual EString GetAttribute(const EString& attrName);//获取属性
	};

	//原理采用PostMessage(请确保当前UI线程中至少存在一个窗口)
	template<class Func, class... Args>
	bool BeginInvoke(Func&& f, Args&& ...args) {
		HWND hWnd = NULL;
		if (!__EzUI__WNDS.empty()) {
			//从全局窗口中获取
			hWnd = *__EzUI__WNDS.begin();
		}
		else {
			return false;
		}
		if (hWnd == NULL) {
			return false;
		}
		std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (::PostMessage(hWnd, WM_GUI_SYSTEM, WM_GUI_BEGININVOKE, (LPARAM)func) == LRESULT(0)) {
			delete func;
			return false;
		}
		return true;
	}
	//原理采用SendMessage(请确保当前UI线程中至少存在一个窗口)
	template<class Func, class... Args>
	bool Invoke(Func&& f, Args&& ...args) {
		std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		if (::GetCurrentThreadId() == EzUI::__EzUI__ThreadId) {
			func();
			return true;
		}
		HWND hWnd = NULL;
		if (!__EzUI__WNDS.empty()) {
			//从全局窗口中获取
			hWnd = *__EzUI__WNDS.begin();
		}
		else {
			return false;
		}
		if (hWnd == NULL) {
			return false;
		}
		if (::SendMessage(hWnd, WM_GUI_SYSTEM, WM_GUI_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
			return false;
		}
		return true;
	}
};
