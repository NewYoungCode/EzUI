#pragma once
#include "Control.h"

namespace EzUI {
	/// <summary>
	/// Window //经典带边框带系统菜单WIN32窗口样式
	/// </summary>
	class UI_EXPORT Window :public IControl
	{
	private:
		//窗口最小尺寸
		Size _miniSize;
		//窗口最大尺寸
		Size _maxSize;
		//上一次客户端大小的信息
		Size _lastSize;
		//上一次移动的坐标
		Point _lastPoint;
		//当窗口关闭的时候退出代码
		int_t _closeCode = 0;
		//基于桌面的坐标
		Rect _rect;
		//客户绘图区域
		Rect _rectClient;
		//所属窗口句柄
		HWND _oWnerWnd = NULL;
		//窗口布局
		Control* _layout = NULL;
		HWND _hWndTips = NULL;
		//是否支持缩放
		bool _resize = false;
		void InitWindow(int_t width, int_t height, HWND owner, DWORD dStyle, DWORD  ExStyle);//初始窗口
	private:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		bool IsInWindow(Control& pControl, Control& it);
	protected:
		//鼠标按下以标题栏方式移动窗口
		void TitleMoveWindow();
		//当dpi发生更改时
		virtual void OnDpiChange(float systemScale, const Rect& newRect);
		//鼠标移动时发生
		virtual void OnMouseMove(const Point& point);
		//鼠标离开时发生
		virtual void OnMouseLeave();
		//鼠标滚动发生
		virtual void OnMouseWheel(int_t zDelta, const Point& point);
		//鼠标双击是发生
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		//鼠标按下时发生
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		//鼠标弹起时发生
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		//鼠标单击并弹起时发生
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);
		//生成渲染器 渲染参数
		virtual void DoPaint(HDC winDC, const Rect& rePaint);
		//渲染中
		virtual void OnPaint(PaintEventArgs& arg);
		//位置发生改变时发生
		virtual void OnLocation(const Point& point);
		//大小发生改变时发生
		virtual void OnSize(const Size& sz);
		//当窗口矩形区域发生改变时发生
		virtual void OnRect(const Rect& rect);
		//当窗口收到WM_CLOSE消息时发生 
		virtual void OnClose(bool& bClose);
		//当窗口销毁时发生
		virtual void OnDestroy();
		//字符消息
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);
		//键盘按下
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		//键盘抬起
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		//获得输入焦点时发生
		virtual void OnFocus(HWND hWnd);
		//失去输入焦点时发生
		virtual void OnKillFocus(HWND hWnd);
		//鼠标 键盘 重绘 会进入此函数,bHandle如果设置位true将不再派发给sender控件处理
		virtual void OnNotify(Control* sender, EventArgs& args,bool& bHandle);
	public:
		//处理消息队列的
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Window(int_t width, int_t height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();

		/// 在窗口中寻找命中的控件 //根据坐标获取控件
		Control* FindControl(const Point clientPoint, Point* outPoint);

		//使用id寻找控件
		Control* FindControl(const EString& objectName);

		//获取窗口句柄
		HWND Hwnd();

		//获取窗口基于显示器的矩形
		const Rect& GetWindowRect();

		//获取客户区矩形
		const Rect& GetClientRect();

		//获取当前窗口dpi缩放系数
		float GetScale();

		//设置窗口size
		void SetSize(const Size& size);

		//设置窗口位置
		void SetLocation(const Point& pt);

		//设置窗口位置大小
		void SetRect(const Rect& rect);

		//设置窗口最小size
		void SetMiniSize(const Size& size);

		//设置窗口最大size
		void SetMaxSize(const Size& size);

		//设置窗口icon
		void SetIcon(short id);

		//设置窗口icon
		void SetIcon(HICON icon);

		//设置窗口主布局
		void SetLayout(EzUI::Control* layout);

		//获取窗口主布局
		Control* GetLayout();

		//设置窗口标题
		void SetText(const EString& text);

		//获取窗口标题
		EString GetText();

		//设置与取消窗口置顶
		void SetTopMost(bool top);

		//设置窗口缩放支持
		void SetResizable(bool resize);

		//是否支持调整大小
		bool IsResizable();

		//窗口是否置顶
		bool IsTopMost();

		//操作窗口的显示
		virtual void Show();

		//操作窗口的显示 带参数
		void Show(int_t cmdShow);

		//隐藏窗口
		virtual void Hide();

		//正常显示窗口
		void ShowNormal();

		//关闭窗口 exitCode为退出代码
		void Close(int_t exitCode = 0);

		//模态窗口方式显示窗口(会阻塞)
		virtual int_t ShowModal(bool disableOnwer = true);

		//最小化窗口
		void ShowMinimized();

		//最大化窗口
		void ShowMaximized();

		//让窗口占满当前屏幕
		void ShowFullScreen();

		//窗口是否显示
		bool IsVisible();

		//设置窗口显示/隐藏
		void SetVisible(bool flag);

		//使区域无效(延迟刷新)
		void Invalidate();

		//立即更新所有无效区域(立即刷新)
		void Refresh();

		//居中到屏幕
		void CenterToScreen();

		//参考某个窗口进行居中
		void CenterToWindow(HWND wnd = NULL);

		//给指定控件为焦点控件
		void SetFocus(Control* ctl);
	};
};