#pragma once
#include "EzUI.h"
#include "Painter.h" 
namespace EzUI {
	enum  Event :int {
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
		OnKillFocus = 2048
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
		WAIT = (ULONG_PTR)IDC_WAIT// 沙漏，Windows7下会显示为选择的圆圈表示等待
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
	//控件样式
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
	//控件行为
	enum class ControlAction {
		None,
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};

#define UIFunc std::function
#define EventMouseMove  UIFunc<void(Control*,const Point&)>  //移动事件
#define EventMouseEnter  UIFunc<void( Control*, const Point&)>//移入事件
#define EventMouseWheel   UIFunc<void( Control*, short, const Point&)>//滚轮事件
#define EventMouseLeave  UIFunc<void( Control*)>//鼠标离开事件
#define EventMouseDown  UIFunc<void( Control*, MouseButton, const Point&)> //鼠标按下事件
#define EventMouseUp  UIFunc<void ( Control*, MouseButton, const Point&)>//鼠标抬起
#define EventMouseClick   UIFunc<void( Control*, MouseButton, const Point&)>//鼠标单击
#define EventMouseDoubleClick   UIFunc<void( Control*, MouseButton, const Point&)>//鼠标双击
#define EventScrollRolling   UIFunc<void(int, int)>//滚动条滚动事件

	typedef std::map<EString, EString> Attributes;
	typedef std::map<EString, EString>::iterator AttributeIterator;
	class Control;
	class ScrollBar;
	class Spacer;
	typedef std::list<Control*> Controls;
	typedef std::list<Control*>::iterator ControlIterator;

	//坐标系
	class UI_EXPORT IRect {
	protected:
		Rect _rect;
		int _fixedWidth = 0;
		int _fixedHeight = 0;
	public:
		DockStyle Dock = DockStyle::None;//dock样式
	public:
		const Rect ClipRect;//控件在窗口中的可见区域
		const int& X();
		const int& Y();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//移动相对与父控件的位置
		void SetSize(const Size& size); //当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetWidth(const int& width);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetHeight(const int& height);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		const int& Width();
		const int& Height();
		void SetFixedWidth(const int& fixedWidth);//设置绝对宽度
		void SetFixedHeight(const int& fixedHeight);//设置绝对高度
		const int& GetFixedWidth();//获取绝对宽度
		const int& GetFixedHeight();//获取绝对高度
		const Rect& GetRect();//获取相对与父控件矩形
		virtual void OnLayout(const Size& parentRect, bool instantly = true);//父控件大小改变事件  instantly立即生效
	public:
		virtual void ComputeClipRect() = 0;//计算基于父控件的裁剪区域
		virtual Rect GetClientRect() = 0;//获取基于客户端的矩形
		virtual void SetRect(const Rect& rect, bool rePaint = false) = 0;//设置相对父控件矩形
	public:
		virtual ~IRect() {};
	};
	//鼠标键盘系
	class UI_EXPORT IMouseKeyBoard {
	protected:
		Tuple<LPTSTR> _LastCursor;
	public:
		int MousePassThrough = 0;
		Cursor Cursor = Cursor::None;//鼠标样式
	public:
		//鼠标事件相关
		EventMouseMove MouseMove;//移动事件
		EventMouseEnter MouseEnter;//移入事件
		EventMouseWheel MouseWheel;//滚轮事件
		EventMouseLeave MouseLeave;//鼠标离开事件
		EventMouseDown MouseDown;//鼠标按下事件
		EventMouseUp MouseUp;//鼠标抬起
		EventMouseClick MouseClick;//鼠标单击
		EventMouseDoubleClick MouseDoubleClick;//鼠标双击
	public:
		virtual ~IMouseKeyBoard() {};
		bool CheckEventPassThrough(Event eventType);
	};

	class UI_EXPORT  IScroll {
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

	class  UI_EXPORT IControl {
	protected:
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//鼠标是否已经按下
		bool _hasTimer = false;
		Attributes _attrs;
	public:
		UINT_PTR Tag = NULL;
		HWND _hWnd = NULL;//if son is control,the hwnd is parent window handle
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
		virtual const EString GetAttribute(const EString& attrName);//获取属性
		virtual UINT_PTR SetTimer(size_t interval);
		virtual void KillTimer();
	};

};