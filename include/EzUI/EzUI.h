// ---------------------------------------------------------------------------
// EzUI 框架
// 版权所有 (c) 2021-N  yang/NewYoungCode (https://github.com/NewYoungCode/EzUI)
//
// EzUI 是一个基于 Win32 消息机制和 Direct2D 渲染的轻量级桌面 UI 框架，
// 支持弹性布局、伪类样式、事件分发、高分辨率适配、分层窗口等特性。
//
// Author: yang/NewYoungCode
// Email: 19980103ly@gmail.com / 718987717@qq.com
// ---------------------------------------------------------------------------

#pragma once

#include "UIDef.h"
#include "UIString.h"
#include "Resource.h"
#include "GraphicsTypes.h"
#include "D2DGraphics.h"

#undef LoadCursor
#undef LoadIcon

namespace ezui {
	class Object;
	enum class Cursor : ULONG_PTR;
	struct MonitorInfo;
	class EventArgs;
	class ControlStyle;
	class Bitmap;
	class Window;

	class Control;
	class Frame;
	class Spacer;
	class ScrollBar;
	class VScrollBar;
	class HScrollBar;
	class TabLayout;
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
	class LayeredWindow;

	/*============================================================枚举============================================================*/
	//事件类型(事件个数不得超过31个)
	enum class Event : int32_t {
		None = 0,
		OnMouseWheel = 1,
		OnMouseEnter = 2,
		OnMouseMove = 4,
		OnMouseLeave = 8,
		OnMouseDoubleClick = 16,
		OnMouseDown = 32,
		OnMouseUp = 64,
		OnKeyDown = 128,
		OnKeyUp = 256,
		OnPaint = 512,
		OnFocus = 1024,
		OnKillFocus = 2048,
		OnKeyChar = 4096,
		OnMove = 8192,
		OnSize = 16384,
		OnLayout = 32768,
		OnRect = 65536,
		OnDpiChange = 131072,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnMouseDrag = OnMouseDown | OnMouseMove,
		OnMouseEvent = OnMouseWheel | OnMouseEnter | OnMouseMove | OnMouseLeave | OnMouseDoubleClick | OnMouseDown | OnMouseUp,
		OnKeyEvent = OnKeyDown | OnKeyUp | OnKeyChar
	};
	EZUI_ENUM_OPERATORS(Event, int32_t);

	//描述控件行为
	enum class ControlAction :int16_t {
		None,
		Title,//具有移动窗口 双击最大化窗口的行为
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};

	//用于描述控件视觉状态(用于伪样式)
	enum class VisualState :int16_t {
		Auto = 1,   // 自动样式：没有指定样式时，使用当前控件状态对应的样式
		Static = 2,//静态
		Disabled = 4,//禁用状态
		Checked = 8,//选中状态
		Hover = 16,//鼠标悬浮
		Active = 32,//鼠标按住
		Focus = 64//具有焦点时
	};
	EZUI_ENUM_OPERATORS(VisualState, int16_t);

	//描述鼠标按下的键
	enum class MouseButton :int16_t {
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

	//光标类型
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
	/*============================================================枚举============================================================*/

	namespace detail {
		//用于样式
		struct Style
		{
			VisualState m_styleType;
			UIString m_selectorName;
			UIString m_styleStr;
		};
	};

	/// <summary>
	/// Graphics绘图上下文(由D2DRenderTarget实现) 功能: 绘制形状、文本、图像、变换等渲染操作
	/// </summary>
	using Graphics = detail::D2DGraphics;

#if 1
	//控件集合(std::vector<Control*>)
	class UI_EXPORT ControlCollection final : public std::vector<Control*> {
	public:
		ControlCollection() = default;
		~ControlCollection() = default;
		//返回集合中的第一个控件
		Control* First()const {
			return this->empty() ? NULL : this->front();
		}
		//返回集合中的最后一个控件
		Control* Last()const {
			return this->empty() ? NULL : this->back();
		}
		//移除元素
		bool Remove(Control* childCtl) {
			auto itor = std::find(this->begin(), this->end(), childCtl);
			if (itor != this->end()) {
				this->erase(itor);
				return true;
			}
			return false;
		}
		//检查元素是否存在
		bool Contains(Control* childCtl)const {
			auto itor = std::find(this->begin(), this->end(), childCtl);
			return itor != this->end();
		}
		//查找索引
		int IndexOf(Control* childCtl)const
		{
			int pos = 0;
			for (auto itor = this->begin(); itor != this->end(); ++itor)
			{
				if ((*itor) == childCtl) {
					return pos;
				}
				++pos;
			}
			return -1;
		}
		//插入元素
		void Insert(int pos, Control* childCtl) {
			size_t i = 0;
			auto itor = this->begin();
			for (; itor != this->end(); ++itor) {
				if (i == pos) {
					break;
				}
				++i;
			}
			if (itor == this->end()) {
				this->push_back(childCtl);
			}
			else {
				this->insert(itor, childCtl);
			}
		}
	};
#else
	// 控件集合
	class UI_EXPORT ControlCollection : public std::list<Control*> {
	public:
		ControlCollection() = default;
		~ControlCollection() = default;
		// 慎用 链表不适合随机访问，下标越大，遍历越慢 O(n)
		Control* operator[](size_t index) const {
			if (index >= this->size()) {
				return NULL;
			}
			auto it = this->begin();
			std::advance(it, index); // 高效移动迭代器
			return *it;
		}
		//返回集合中的第一个控件
		Control* First() const {
			return this->empty() ? NULL : this->front();
		}
		//返回集合中的最后一个控件
		Control* Last() const {
			return this->empty() ? NULL : this->back();
		}
	};

#endif

	namespace detail {
		//全局资源句柄(内部使用)
		struct GlobalContent {
			HMODULE __EzUI__HINSTANCE = NULL;//全局实例
			Resource* __EzUI__Resource = NULL;//文件中的全局资源句柄
			HWND __EzUI_MessageWnd = NULL;//用于UI通讯的隐形窗口
			DWORD __EzUI__ThreadId = NULL;//UI的线程Id
			std::list<ezui::MonitorInfo> __EzUI__MonitorInfos;//所有监视器信息
		};
		//获取全局上下文(内部使用)
		extern UI_EXPORT detail::GlobalContent* GetGlobalContent();
		//设置全局上下文(内部使用)
		extern UI_EXPORT void SetGlobalContent(detail::GlobalContent*);

		//内部用于计算内容大小的工具
		struct ContentMeasure {
		private:
			int m_minLeft, m_maxRight;
			int m_minTop, m_maxBottom;
		public:
			ContentMeasure();
			void Update(int left, int top, int right, int bottom);
			void Update(const Rect& rect, const Distance& margin);
			int ContentWidth() const;
			int ContentHeight() const;
		};
	};

	//判断两个float是相等(两数是否接近)
	extern UI_EXPORT bool IsFloatEqual(float num1, float num2);
	//加载HICON
	extern UI_EXPORT HICON LoadIcon(const UIString& fileName);
	//装载字体到框架中(从文件或者资源文件中加载字体)
	extern UI_EXPORT bool InstallFont(const UIString& fontFileName, UIString* outFontName = NULL);
	//装载字体到框架中(从内存中加载字体)
	bool InstallFont(const char* data, uint32_t size, UIString* outFontName = NULL);
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

	//存储BGRA (4*8)的颜色
	class UI_EXPORT Color :public ezui::detail::__EzUI__Color {
	public:
		Color(const ezui::detail::__EzUI__Color& copy) { this->BGRA = copy.GetValue(); }
		Color(const uint32_t& bgra = 0) :ezui::detail::__EzUI__Color(bgra) {}
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :ezui::detail::__EzUI__Color(r, g, b, a) {}
	public:
		//构建一个Color
		static Color Make(const UIString& colorStr, bool* isGood = NULL);
		virtual ~Color() {}
	};

	/// <summary>
	/// 描述边框的一些信息
	/// </summary>
	class Border {
	public:
		Value<int16_t> Left;//左边边框大小
		Value<int16_t> Top;//顶部边框大小
		Value<int16_t> Right;//右边边框大小
		Value<int16_t> Bottom;//底部边框大小
		Value<int16_t> TopLeftRadius;
		Value<int16_t> TopRightRadius;
		Value<int16_t> BottomRightRadius;
		Value<int16_t> BottomLeftRadius;
		Value<Color> Color;
		Value<BorderStyle> Style = BorderStyle::None;
	public:
		class Radius {
			Border& Border;
		public:
			Radius(ezui::Border& bd) :Border(bd) {}
			//对四个角度同时设置半径大小
			Radius& operator=(Value<int16_t> radius) {
				Border.TopLeftRadius = radius;
				Border.TopRightRadius = radius;
				Border.BottomRightRadius = radius;
				Border.BottomLeftRadius = radius;
				return *this;
			}
		};
	public:
		Border::Radius Radius = (*this);
	public:
		Border() {
			this->Style.SetEnabled(false);
		}
		//对四个边设置大小
		Border& operator=(Value<int16_t> borderWidth) {
			Left = borderWidth;
			Top = borderWidth;
			Right = borderWidth;
			Bottom = borderWidth;
			return *this;
		}
		void Scale(float scale) {
			Left.Set(Left * scale + 0.5);
			Top.Set(Top * scale + 0.5);
			Right.Set(Right * scale + 0.5);
			Bottom.Set(Bottom * scale + 0.5);
			TopLeftRadius.Set(TopLeftRadius * scale + 0.5);
			TopRightRadius.Set(TopRightRadius * scale + 0.5);
			BottomRightRadius.Set(BottomRightRadius * scale + 0.5);
			BottomLeftRadius.Set(BottomLeftRadius * scale + 0.5);
		}
	};


#if USED_DIRECT2D
	class UI_EXPORT Image :public detail::DXImage {
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
		//从本地路径加载图片
		Image(const std::wstring& fileName) :DXImage(fileName) {}
		Image(const void* data, size_t dataCount) :DXImage(data, dataCount) {}
		Image(detail::DXImage* dxImage) :detail::DXImage(dxImage) {}
		virtual Image* Clone() override;
	public:
		//从资源或者本地文件自动构建一个Image
		static Image* Make(const UIString& fileOrRes);
	};
#endif
	// 定义用于保存显示器信息的结构体
	struct MonitorInfo {
		//显示器缩放比例 1.0 1.25 1.5 1.75 2.0
		float Scale = 1.0f;
		//显示器帧率
		float FPS = 60;
		//是否为主显示器
		bool Primary = false;
		HMONITOR Monitor = NULL;
		//显示器物理宽高
		Size Physical;
		//显示器的位置 多显示器下Y轴可能出现负数或者大于0的时候代表显示器在设置里面显示器是错位的(多显示器没有平行);
		//逻辑宽高
		ezui::Rect Rect;
		//工作区域
		ezui::Rect WorkRect;
	};

	//窗口公共数据
	struct WindowContext {
		//缩放率
		float Scale = 1.0f;
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
		//清空控件标记等等...
		std::function<void(Control*)> CleanControl = NULL;
		//内部移动窗口的函数
		std::function<void()> MoveWindow = NULL;
		//内部使用标题部分移动窗口的函数
		std::function<void()> TitleMoveWindow = NULL;
		//处理消息过程的回调函数
		std::function<LRESULT(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> WndProc = NULL;
		//设置焦点控件
		std::function<void(Control*)> SetFocus = NULL;
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
	//为按键事件提供基础数据
	class UI_EXPORT KeyEventArgs :public EventArgs {
	public:
		//按下的键值
		WPARAM KeyCode;
		LPARAM lParam;
		KeyEventArgs(Event eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType) {
			this->KeyCode = wParam;
			this->lParam = lParam;
		}
		virtual ~KeyEventArgs() {}
	};
	//为焦点相关事件提供基础数据
	class UI_EXPORT FocusEventArgs :public EventArgs {
	public:
		//上一个具有焦点的控件
		Control* Control;
		FocusEventArgs(Event eventType, ezui::Control* ctl) :EventArgs(eventType) {
			this->Control = ctl;
		}
		virtual ~FocusEventArgs() {}
	};
	//坐标发生改变
	class UI_EXPORT MoveEventArgs :public EventArgs {
	public:
		const ezui::Point Location;
		MoveEventArgs(const ezui::Point& location) :EventArgs(Event::OnMove), Location(location) {}
		virtual ~MoveEventArgs() {}
	};
	//大小发生改变
	class UI_EXPORT SizeEventArgs :public EventArgs {
	public:
		const ezui::Size Size;
		SizeEventArgs(const ezui::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//dpi发生变化
	class UI_EXPORT DpiChangedEventArgs :public EventArgs {
	public:
		float Scale = 1.0f;
		DpiChangedEventArgs(float scale) :EventArgs(Event::OnDpiChange), Scale(scale) {}
		virtual ~DpiChangedEventArgs() {}
	};
	// 为 OnPaint 事件提供数据。
	class UI_EXPORT PaintEventArgs :public EventArgs {
	private:
		std::list<bool> m_layers;
		std::list<Point> m_offsets;
	public:
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		const WindowContext* WindowContext = NULL;
		::HWND HWND = NULL;
		ezui::Graphics& Graphics;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		PaintEventArgs(ezui::Graphics& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
		//添加裁剪(速度较快)
		void PushLayer(const Rect& rectBounds);
		//添加异形裁剪 比较耗性能,但是可以异形抗锯齿裁剪
		void PushLayer(const Geometry& dxGeometry, float opacity);
		//弹出最后一个裁剪
		void PopLayer();
		//放入一个偏移
		void PushOffset(const Point& offset);
		//弹出最后一个偏移
		void PopOffset();
	};
	// 为控件样式提供数据。
	class UI_EXPORT ControlStyle final {
	public:
		//边框信息
		ezui::Border& Border;
		//背景颜色
		Value<Color> BackColor;
		//背景图片 如果指定的图片被删除 请必须将此置零
		Value<Image*> BackImage;
		//前景图片 如果指定的图片被删除 请必须将此置零
		Value<Image*> ForeImage;
		//字体名称 具有父控件继承性
		Value<std::wstring> FontFamily;
		//字体大小 具有父控件继承性
		Value<int> FontSize;
		//字体粗度 值范围1~999 如需加粗一般为700即可 具有继承性值
		Value<int> FontWeight;
		//字体样式 具有父控件继承性
		Value<FontStyle> FontStyle;
		//前景颜色 (通常应用于字体颜色)具有父控件继承性
		Value<Color> ForeColor;
		//鼠标样式
		Value<HCURSOR> Cursor;
		//正数角度(0~ 360) -> 逆时针旋转
		//负数角度(0~ -360) -> 顺时针旋转
		Value<float> Angle;
		//透明度(0~1.0)
		Value<float> Opacity;
	private:
		void operator=(const ControlStyle&) = delete;//禁止直接赋值
		ControlStyle(const ControlStyle&) = delete; //禁止拷贝 
	public:
		ControlStyle() :Border(*(new ezui::Border)) {
			Angle.Set(std::numeric_limits<float>::quiet_NaN());
			Opacity.Set(std::numeric_limits<float>::quiet_NaN());
		}
		~ControlStyle() {
			delete (&Border);
		}
		void Scale(float scale);
	};

	namespace detail {
		//指针管理
		template <typename T>
		class PtrManager final {
		private:
			std::vector<T>& m_ptrs;
		public:
			PtrManager() :m_ptrs(*(new std::vector<T>)) {}
			~PtrManager() {
				this->Clear();
				delete (&m_ptrs);
			}
			void Add(const T& v) {
				if (v) {
					auto itor = std::find(m_ptrs.begin(), m_ptrs.end(), v);
					if (itor == m_ptrs.end()) {
						m_ptrs.push_back(v);
					}
				}
			}
			void Remove(const T& v) {
				auto it = std::find(m_ptrs.begin(), m_ptrs.end(), v);
				if (it != m_ptrs.end()) {
					m_ptrs.erase(it);
				}
			}
			void Clear() {
				auto itor = m_ptrs.begin();
				while (itor != m_ptrs.end())
				{
					T item = *itor;
					itor = m_ptrs.erase(itor); // erase 返回下一个有效迭代器
					delete item;
				}
			}
		};
	};

	//常用对象基类
	class UI_EXPORT Object {
	private:
		//属性集合
		std::map<UIString, UIString>* m_attrs = NULL;
		// 管理子对象的释放
		detail::PtrManager<Object*>* m_childObjects = NULL;
		//是否正在被销毁
		bool m_bIsDestroying = false;
		//用户自定义数据
		void* m_userData = NULL;
	private:
		Object(const Object&) = delete;            // 禁止拷贝构造
		Object& operator=(const Object&) = delete; // 禁止拷贝赋值
		Object(Object&&) = delete;                 // 禁止移动构造
		Object& operator=(Object&&) = delete;      // 禁止移动赋值
	public:
		Object(Object* ownerObject = NULL);
		virtual ~Object();
	public:
		//设置用户自定义数据
		void SetUserData(void* userData);
		//获取用户自定义数据
		void* GetUserData();
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
		//对象是否正在被销毁(预防析构降级导致控件访问报错)
		bool IsDestroying();
		//延迟删除
		void DeleteLater();
	};


	//原理采用PostMessage
	template<class Func, class... Args>
	bool BeginInvoke(Func&& f, Args&& ...args) {
		HWND hWnd = ezui::detail::GetGlobalContent()->__EzUI_MessageWnd;
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
		if (::GetCurrentThreadId() == ezui::detail::GetGlobalContent()->__EzUI__ThreadId) {
			func();
			return true;
		}
		HWND hWnd = ezui::detail::GetGlobalContent()->__EzUI_MessageWnd;
		if (hWnd == NULL || !::IsWindow(hWnd)) {
			return false;
		}
		if (::SendMessage(hWnd, WM_GUI_SYSTEM, WM_GUI_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
			return false;
		}
		return true;
	}

	//统计函数耗时
	template<class Func, class... Args>
	int64_t StopWatch(Func&& f, Args&& ...args) {
		auto beginTime = std::chrono::steady_clock::now();
		std::forward<Func>(f)(std::forward<Args>(args)...);
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginTime).count();
		return delta;
	}

};
