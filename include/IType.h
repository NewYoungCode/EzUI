#pragma once
#include "UIDef.h"
#include "EString.h"
#include "RenderType.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"
#pragma comment(lib, "Msimg32.lib")
namespace EzUI {
	using RectF = RenderType::RectF;
	using Size = RenderType::Size;
	using SizeF = RenderType::SizeF;
	using Point = RenderType::Point;
	using PointF = RenderType::PointF;
	using ARGB = RenderType::ARGB;

	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	struct UI_EXPORT Rect :public RenderType::Rect {
	private:
		void StringToRect(const EString& str);
	public:
		Rect() :RenderType::Rect() {};
		virtual ~Rect() {};
		Rect(IN INT x, IN INT y, IN INT width, IN INT height) :RenderType::Rect(x, y, width, height) {};
		Rect(IN const Point& location, IN const Size& size) :RenderType::Rect(location, size) {};
		Rect(const EString& rect);
		Rect& operator=(const EString& rect);
		Rect(const RECT& winRect);
	};
	struct UI_EXPORT Color :RenderType::Color {
	public:
		bool valid = false;
	private:
		void _MakeARGB(const EString& colorStr);
	public:
		Color();
		virtual ~Color() {}
		Color& operator=(const EString& colorStr);
		Color(const EString& colorStr);
		Color(IN BYTE r, IN BYTE g, IN BYTE b);
		Color(IN BYTE a, IN BYTE r, IN BYTE g, IN BYTE b);
		Color(IN ARGB argb);
		Color& operator=(const Color& Align_Right_Color);
		COLORREF COLORRE() const;
	};
	struct UI_EXPORT EBitmap {
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HDC _hdc = NULL;
		HGDIOBJ _hgdiobj = NULL;
		EBitmap(const EBitmap& hBitmap) {}
		void operator=(const EBitmap& hBitmap) {}
	public:
		HBITMAP _bitmap = NULL;
		WORD Width;
		WORD Height;
		void* point = NULL;
		BITMAPINFO bmi;
		byte biteCount = 0;
		EBitmap(WORD width, WORD height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		HDC& GetDC();
		virtual ~EBitmap();
	};

	template<typename T>
	struct Tuple {
	public:
		bool valid = false;
		T value = NULL;
		Tuple() :valid(false) {
		}
		Tuple(const T& value) {
			this->value = value;
			valid = true;
		}
		operator T () {
			return value;
		}
		T& operator->() {
			return value;
		}
	};
	template<typename T>
	struct SafeObject {
	private:
		SafeObject(const SafeObject& right) {}
		SafeObject& operator=(const SafeObject& right) {}
	public:
		T* ptr = NULL;
		SafeObject(T* _ptr) {
			ptr = _ptr;
		}
		operator T* () {
			return ptr;
		}
		~SafeObject() {
			if (ptr) delete ptr;
		}
	};

	struct WindowData {
		void* Window = NULL;//窗口类实例
		void* HoverStyles = NULL;
		void* ActiveStyles = NULL;//
		bool Debug = false;//是否开启debug模式
		HWND HANDLE = NULL;//窗口句柄
		UIFunc<void(void*)> InvalidateRect = NULL;//使一个区域无效
		UIFunc<void()> UpdateWindow = NULL;//立即更新全部无效区域
		UIFunc<bool(UINT, WPARAM, LPARAM)> Notify = NULL;//
		UIFunc<void(Control*, const std::wstring&)> SetTips = NULL;//设置悬浮提示文字
		UIFunc<void(Control*)> DelTips = NULL;//移除悬浮提示文字
		UIFunc<Cursor()> GetCursor = NULL;//获取鼠标样式
		UIFunc<void(Cursor)> SetCursor = NULL;//设置鼠标样式
		UIFunc<void(Control*)> RemoveControl = NULL;//清空控件标记等等...
	};
	struct StopWatch {
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
			return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		}
	};

	enum Event :int {
		OnMouseWheel = 1,
		OnMouseEnter = 2,
		OnMouseMove = 4,
		OnMouseLeave = 8,
		OnMouseClick = 16,
		OnMouseDoubleClick = 32,
		OnMouseDown = 64,
		OnMouseUp = 128,
		OnKeyDown = 256,
		OnKeyUp = 512,
		OnPaint = 1024,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnKillFocus = 2048,
	};
	enum class ControlAction {
		None,
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};
	enum class ControlState {
		None,
		Hover,
		Active,
		Disable
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
		None = 0,
		//
		// 摘要: 
		//     鼠标左按钮曾按下。
		Left = 1048576,
		//
		// 摘要: 
		//     鼠标右按钮曾按下。
		Right = 2097152,
		//
		// 摘要: 
		//     鼠标中按钮曾按下。
		Middle = 4194304,
		//
		// 摘要: 
		//     第 1 个 XButton 曾按下。
		XButton1 = 8388608,
		//
		// 摘要: 
		//     第 2 个 XButton 曾按下。
		XButton2 = 16777216,
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
		WAIT = (ULONG_PTR)IDC_WAIT,// 沙漏，Windows7下会显示为选择的圆圈表示等待
		ALL = APPSTARTING | ARROW | CROSS | HAND | HELP | IBEAM | ICON | NO | SIZE | SIZEALL | SIZENESW | SIZENS | SIZENWSE | SIZEWE | UPARROW | WAIT
	};
	// 摘要: 
	//基础事件
	struct EventArgs {
		Event EventType;
	};
	// 摘要: 
	//为鼠标事件提供基础数据
	struct MouseEventArgs :public EventArgs {
	public:
		MouseButton Button;
		short Delta;
		Point Location;
	public:
		MouseEventArgs() {}
		virtual ~MouseEventArgs() {}
		MouseEventArgs(const Event& eventType, const Point& location = Point(0, 0), const MouseButton& mouseButton = MouseButton::None, const short& delta = 0) {
			this->EventType = eventType;
			this->Button = mouseButton;
			this->Delta = delta;
			this->Location = location;
		}
	};

#if USED_GDIPLUS
	using Painter = GdiplusRender;
	using Image = GdiplusImage;
#endif
#if USED_Direct2D
	using Painter = Direct2DRender;
	using Image = DXImage;
#endif
	// 摘要: 
	// 为 OnPaint 事件提供数据。
	using __Painter = Painter;
	struct PaintEventArgs {
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		Painter& Painter;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		Rect ClipRect;//上一个可见裁剪区域
		HWND HWnd;//父窗口句柄
		PaintEventArgs(__Painter& painter) :Painter(painter) {}
		virtual ~PaintEventArgs() {}
	};


	typedef Tuple<float> UI_Float;
	typedef Tuple<int> UI_Int;
	typedef Tuple<size_t> UI_UInt;//
	typedef Tuple<Image*> HImage;
	typedef std::map<EString, EString> Attributes;//属性集合
	typedef std::map<EString, EString>::iterator AttributeIterator;
	typedef std::list<Control*> Controls;//控件集合
	typedef std::list<Control*>::iterator ControlIterator;//
	typedef UIFunc<void(Control*, const Point&)> EventMouseMove;  //移动事件
	typedef UIFunc<void(Control*, const Point&)> EventMouseEnter;//移入事件
	typedef UIFunc<void(Control*, short, const Point&)> EventMouseWheel;//滚轮事件
	typedef UIFunc<void(Control*)> EventMouseLeave;//鼠标离开事件
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseDown; //鼠标按下事件
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseUp;//鼠标抬起
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseClick;//鼠标单击
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseDoubleClick;//鼠标双击
	typedef UIFunc<void(int, int)> EventScrollRolling;//滚动条滚动事件
	typedef UIFunc<bool(PaintEventArgs& args)> EventPaint;//绘制
	typedef UIFunc<bool(PaintEventArgs& args)> EventBackgroundPaint;//背景绘制 背景图片&颜色
	typedef UIFunc<bool(PaintEventArgs& args)> EventForePaint;//前景绘制 前景图片&文字

	class UI_EXPORT ControlStyle {
	public:
		//UI_Float Opacity;//整体不透明度
		UI_Int Radius;//圆角系数
		UI_Int BorderLeft;//左边边框
		UI_Int BorderTop;//顶部边框
		UI_Int BorderRight;//右边边框
		UI_Int BorderBottom;//底部边框
		Color BorderColor;//边框颜色
		Color BackgroundColor;//背景颜色
		HImage BackgroundImage;//背景图片
		HImage ForeImage;//前景图片

		EString FontFamily;//字体名称   具有继承性
		UI_Int FontSize;//字体大小       具有继承性
		Color ForeColor;//前景颜色      具有继承性
	private:
		void operator=(const ControlStyle& right) {} //禁止直接赋值 因为这样会导致 Color执行拷贝使得Color变得不合法的有效
		ControlStyle(const ControlStyle& right) {} //禁止拷贝 因为这样会导致 Color执行拷贝使得Color变得不合法的有效
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void SetBorder(const Color& color, int width);
		bool IsValid();
		void SetStyleSheet(const EString& styleStr);
		void SetStyle(const EString& key, const EString& value);
	};
	class UI_EXPORT ILayout {
	public:
		bool PendLayout = true;//布局是否被挂起 当Addcontrol或者RemoveControl的时候此标志为true 当调用ResumeLayout()之后此标志为false
	public:
		virtual void ResumeLayout() = 0;//调用此函数之后请将 PendLayout 设置成false
		virtual ~ILayout() {};
	};
	class UI_EXPORT IScroll {
	public:
		int Margin = 0;//如果是水平列表那就是 右边距 ,垂直滚动条就是下边距
		std::map<Control*, int> LocationX;
		std::map<Control*, int> LocationY;
		virtual ~IScroll() {};
	};
	class UI_EXPORT IScrollBar {
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual  int RollingCurrent() = 0;
		virtual int RollingTotal() = 0;//
		EventScrollRolling Rolling = NULL;
		virtual ~IScrollBar() {};
	};
	class UI_EXPORT IControl {
	protected:
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//鼠标是否已经按下
		Attributes _attrs;
	public:
		UINT_PTR Tag = NULL;
	public:
		IControl();
		virtual ~IControl();
	public:
		virtual void OnMouseMove(const Point& point) = 0;
		virtual void OnMouseLeave() = 0;
		virtual void OnMouseWheel(short zDelta, const Point& point) = 0;
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point) = 0;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point) = 0;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point) = 0;
		virtual void OnSize(const Size& size) = 0;
		virtual void OnLoad() = 0;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) = 0;
		virtual void OnKeyDown(WPARAM wParam) = 0;
		virtual void OnTimer();//计时器函数
	public:
		virtual void SetStyleSheet(const EString& styleStr);//设置style
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//设置属性
		virtual EString GetAttribute(const EString& attrName);//获取属性
	};

};