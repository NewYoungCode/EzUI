#pragma once
#include "Control.h"

namespace EzUI {
	/// <summary>
	/// Window //经典带边框带系统菜单WIN32窗口样式
	/// </summary>
	class UI_EXPORT Window :public IControl
	{
	private:
		Rect _downRect;//上次按下的区域
		Size _miniSize;//窗口最小尺寸
		Size _maxSize;//窗口最大尺寸
		bool _mouseIn = false;//鼠标是否在里面
		bool _mouseDown = false;//鼠标是否已经按下
		bool _moveWindow = false;//窗口移动
		POINT _dragPoint;//记录鼠标坐标
		ULONGLONG _lastDownTime = 0;//上一次鼠标按下的时间
		Size _lastSize;//上一次客户端大小的信息
		Point _lastPoint;//上一次移动的坐标
		MouseButton _lastBtn = MouseButton::None;//上一次鼠标按下的按钮
		int _closeCode = 0;//当窗口关闭的时候退出代码
		Rect _rect;//基于桌面的坐标
		Rect _rectClient;//客户绘图区域
		HWND _oWnerWnd = NULL;//所属窗口句柄
		Control* _layout = NULL;//窗口布局
		HWND _hWndTips = NULL;
		void InitWindow(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle);//初始窗口
	private:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		bool IsInWindow(Control& pControl, Control& it);
	protected:
		void MoveWindow();//鼠标按下移动窗口
		virtual void OnDpiChange(const float& systemScale, const Rect& newRect);//当dpi发生更改时
		virtual void OnMouseMove(const Point& point);//鼠标移动时发生
		virtual void OnMouseLeave();//鼠标离开时发生
		virtual void OnMouseWheel(int zDelta, const Point& point);//鼠标滚动发生
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//鼠标双击是发生
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//鼠标按下时发生
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//鼠标弹起时发生
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//鼠标单击并弹起时发生
		virtual void DoPaint(HDC winDC, const Rect& rePaint);//生成渲染器 渲染参数
		virtual void OnPaint(PaintEventArgs& arg);//渲染中
		virtual void OnLocation(const Point& point);//位置发生改变时发生
		virtual void OnSize(const Size& sz);//大小发生改变时发生
		virtual void OnRect(const Rect& rect);//当窗口矩形区域发生改变时发生
		virtual void OnClose(bool& bClose);//当窗口收到WM_CLOSE消息时发生 
		virtual void OnDestroy();//当窗口销毁时发生
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);//字符消息
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);//键盘按下
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//键盘抬起
		virtual void OnFocus(HWND hWnd);//获得输入焦点时发生
		virtual void OnKillFocus(HWND hWnd);//失去输入焦点时发生
		//鼠标 键盘 重绘 会进入此函数,返回true将不再派发给sender控件处理 注意:尽量不要在此函数内部删除自身控件(鼠标和键盘事件内可以删除) 相当于QT的eventFilter
		//return true将会拦截后续的处理逻辑 请谨慎使用
		virtual bool OnNotify(Control* sender, EventArgs& args);
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//处理消息队列的
	public:
		bool Zoom = true;//是否支持缩放
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();
		/// <summary>
		/// 在窗口中寻找命中的控件
		/// </summary>
		/// <param name="clientPoint">输入的坐标点寻找控件</param>
		/// <param name="outPoint">返回控件的相对坐标</param>
		/// <returns>返回命中的控件指针</returns>
		Control* FindControl(const Point clientPoint, Point* outPoint);//根据坐标获取控件
		Control* FindControl(const EString& objectName);//使用id寻找控件
		HWND Hwnd();//获取窗口句柄
		const Rect& GetWindowRect();//获取窗口基于显示器的矩形
		const Rect& GetClientRect();//获取客户区矩形
		const float& GetScale();//获取当前窗口dpi缩放
		void SetSize(const Size& size);//设置窗口size
		void SetLocation(const Point& pt);//设置窗口位置
		void SetRect(const Rect& rect);//设置窗口矩形
		void SetMiniSize(const Size& size);//设置窗口最小size
		void SetMaxSize(const Size& size);//设置窗口最大size
		void SetIcon(short id);//设置窗口icon
		void SetIcon(HICON icon);//设置窗口icon
		void SetLayout(EzUI::Control* layout);//设置窗口主布局
		Control* GetLayout();//获取窗口主布局
		void SetText(const EString& text);//设置窗口标题
		EString GetText();//获取窗口标题
		void SetTopMost(bool top);//设置与取消窗口置顶
		bool IsTopMost();//窗口是否置顶
		virtual void Show();//操作窗口的显示
		void Show(int cmdShow);//
		virtual void Hide();//隐藏窗口
		void ShowNormal();//正常显示窗口
		void Close(int exitCode = 0);//关闭窗口 exitCode为退出代码
		virtual int ShowModal(bool disableOnwer = true);//会阻塞
		void ShowMinimized();//最小化窗口
		void ShowMaximized();//最大化窗口
		void ShowFullScreen();//让窗口占满当前屏幕
		bool IsVisible();//窗口是否显示
		void SetVisible(bool flag);
		void Invalidate();//使区域无效(延迟刷新)
		void Refresh();//立即更新所有无效区域(立即刷新)
		void CenterToScreen();//居中到屏幕
		void CenterToWindow(HWND wnd = NULL);//参考某个窗口进行居中
		void SetFocus(Control* ctl);//给指定控件为焦点控件
	};
};