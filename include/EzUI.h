#pragma once
#include "UIDef.h"
#include "EString.h"
#include "unzip.h"
#include "RenderType.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"
namespace EzUI {

	//全局资源句柄
	extern UI_EXPORT HZIP HZipResource;//zip文件中的全局资源句柄
	extern UI_EXPORT HGLOBAL HVSResource;//vs中的资源文件句柄
	//获取当前线程ID
	extern UI_EXPORT size_t GetThreadId();
	//获取当前线程注册的窗口类名
	extern UI_EXPORT StdString GetThisClassName();
	//从全局zip获取资源
	extern UI_EXPORT bool GetGlobalResource(const EString& fileName, std::string** outData);
	//从全局zip获取资源
	extern UI_EXPORT bool GetGlobalResource(const EString& fileName, IStream** outData);

	using RectF = RenderType::RectF;
	using Size = RenderType::Size;
	using SizeF = RenderType::SizeF;
	using Point = RenderType::Point;
	using PointF = RenderType::PointF;
	using ARGB = RenderType::ARGB;

	class EventArgs;
	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	class UI_EXPORT Rect :public RenderType::Rect {
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
	class UI_EXPORT Color :public RenderType::Color {
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
	class UI_EXPORT EBitmap {
	public:
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HDC _hdc = NULL;
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
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y);
		void Earse(const Rect& rect);//抹除矩形内容
		void FillRect(const Rect& rect, const Color& color);//

		HDC& GetDC();
		virtual ~EBitmap();
	};

#if USED_GDIPLUS
	class Image :public GdiplusImage {
	public:
		Image(HBITMAP hBitmap) :GdiplusImage(hBitmap) {}
		Image(IStream* iStream) :GdiplusImage(iStream) {}

		Image(const EString& fileOrRes) :GdiplusImage((IStream*)NULL) {
			IStream* iStream = NULL;
			if (GetGlobalResource(fileOrRes, &iStream)) {
				this->CreateFormStream(iStream);
				iStream->Release();
			}
			else {
				this->CreateFromFile(fileOrRes.utf16());
			}
		}
	};
#endif
#if USED_Direct2D
	class Image :public DXImage {
	public:
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const EString& fileOrRes) {
			IStream* iStream = NULL;
			if (GetGlobalResource(fileOrRes, &iStream)) {
				this->CreateFormStream(iStream);
				iStream->Release();
			}
			else {
				this->CreateFromFile(fileOrRes.utf16());
			}
		}
	};
#endif

	template<typename T>
	class Tuple {
	private:
		T value = NULL;
	public:
		bool valid = false;
		Tuple() :valid(false) {
		}
		Tuple(const T& value) {
			this->value = value;
			valid = true;
		}
		operator T () {
			return value;
		}
		T operator->() {
			return value;
		}
	};
	template<typename T>
	class SafeObject {
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
	//感觉c#的委托很好用 所以抄袭一个
	template<class out, class...in>
	class Delegate {
		using TFunc = std::function<out(in...)>;
		std::map<size_t, TFunc> callbacks;
	public:
		auto operator+=(const TFunc& callback) {
			auto pair = std::pair<size_t, TFunc>(size_t(&callback), callback);
			callbacks.emplace(pair);
			return size_t(&callback);
		}
		void operator-=(size_t eventid) {
			callbacks.erase(eventid);
		}
		void operator()(in... args) {
			for (auto& it : callbacks) {
				it.second(std::forward<in>(args)...);
			}
		}
	};
	struct WindowData {
		void* Window = NULL;//窗口类实例
		Control* FocusControl = NULL;//具有焦点的控件
		Control* InputControl = NULL;//输入框
		bool Debug = false;//是否开启debug模式
		HWND HANDLE = NULL;//窗口句柄
		UIFunc<void(void*)> InvalidateRect = NULL;//使一个区域无效
		UIFunc<void()> UpdateWindow = NULL;//立即更新全部无效区域
		UIFunc<bool(Control*, EventArgs&)> Notify = NULL;//
		UIFunc<void(Control*, const std::wstring&)> SetTips = NULL;//设置悬浮提示文字
		UIFunc<void(Control*)> DelTips = NULL;//移除悬浮提示文字
		UIFunc<void(Control*)> RemoveControl = NULL;//清空控件标记等等...
	};
	class StopWatch {
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

	extern size_t __count_onsize;

	enum class LayoutState :byte {
		None, //无状态 (无需布局)
		Pend,//挂起中
		Layouting//布局中
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
		OnKillFocus = 2048,
		OnChar = 4096,
		OnLocation = 8192,
		OnSize = 16384,
		OnTextChange = 32768,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave
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
	class EventArgs {
	public:
		Event EventType;
		virtual ~EventArgs() {};
	};
	// 摘要: 
	//为鼠标事件提供基础数据
	class MouseEventArgs :public EventArgs {
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
	// 摘要: 
	//为键盘事件提供基础数据
	class KeyboardEventArgs :public EventArgs {
	public:
		/// <summary>
		/// 一般是指 键盘的ascii值
		/// </summary>
		WPARAM wParam;
		LPARAM lParam;
		KeyboardEventArgs(const Event& eventType, WPARAM wParam, LPARAM lParam) {
			this->EventType = eventType;
			this->wParam = wParam;
			this->lParam = lParam;
		}
	};
	//坐标发生改变
	class LocationEventArgs :public EventArgs {
	public:
		EzUI::Point PrevLocation;
		EzUI::Point Location;
		LocationEventArgs() {
			this->EventType = Event::OnLocation;
		}
	};
	//大小发生改变
	class SizeEventArgs :public EventArgs {
	public:
		EzUI::Size PrevSize;
		EzUI::Size Size;
		SizeEventArgs() {
			this->EventType = Event::OnSize;
		}
	};
	// 摘要: 
	// 为 OnPaint 事件提供数据。
	class PaintEventArgs :public EventArgs {
	public:
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::Painter& Painter;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		PaintEventArgs(EzUI::Painter& painter) :Painter(painter) {
			EventType = Event::OnPaint;
		}
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
	typedef UIFunc<void(PaintEventArgs&)> EventPaint;//绘制
	typedef UIFunc<void(EString)> EventTextChange;//文字变更事件

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
		ControlStyle(const ControlStyle& right) {} //禁止拷贝 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void SetBorder(const Color& color, int width);
		bool IsValid();
		void SetStyleSheet(const EString& styleStr);
		void SetStyle(const EString& key, const EString& value);
	};
	class UI_EXPORT IScroll {
	public:
		std::map<Control*, int> LocationX;
		std::map<Control*, int> LocationY;
		virtual ~IScroll() {};
	};
	class UI_EXPORT IScrollBar {
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual int RollingCurrent() = 0;
		virtual int RollingTotal() = 0;//
		virtual void OwnerSize(const Size& parentSize) = 0;
		EventScrollRolling Rolling = NULL;//滚动事件
		virtual ~IScrollBar() {};
	};
	class UI_EXPORT IControl {
	private:
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
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) = 0;
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) = 0;
	public:
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//设置属性
		virtual EString GetAttribute(const EString& attrName);//获取属性
	};

	namespace Debug {
		template<typename ...T>
		inline void Log(const EString& formatStr, T ...args) {
#ifdef DEBUGLOG
			char buf[1025]{ 0 };
			auto count = sprintf_s((buf), 1024, formatStr.c_str(), std::forward<T>(args)...);
			buf[count] = '\n';
			buf[count + 1] = NULL;
			OutputDebugStringW(EString(buf).utf16().c_str());
#endif
		}
	};
};