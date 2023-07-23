#pragma once
#include "Control.h"
#include "TextBox.h"

namespace EzUI {
	/// <summary>
	/// Window //经典带边框带系统菜单WIN32窗口样式
	/// </summary>
	class UI_EXPORT Window :public IControl
	{
	private:
		Rect _downRect;
		Size _miniSize;
		Size _maxSize;
		bool _mouseIn = false;
		bool _mouseDown = false;//鼠标是否已经按下
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);
		Size _lastSize;//上一次客户端大小的信息
		Point _lastPoint;//上一次移动的坐标
		MouseButton _lastBtn = MouseButton::None;
		int _closeCode = 0;
		HWND _OwnerHwnd = NULL;
		HWND _hWnd = NULL;
		Rect _rect;//基于桌面的坐标
		Rect _rectClient;//客户绘图区域
		void InitData(const DWORD& ExStyle);
	public:
		WindowData PublicData;//存储公共数据
	private:
		Window(const Window&) {};
		bool IsInWindow(Control& pControl, Control& it);
		bool FindControl(Control* nodeCtl, Control* findControl);
	protected:
		LRESULT ZoomWindow(const LPARAM& lParam);//缩放窗口
		void MoveWindow();//鼠标按下移动窗口
		virtual void OnMouseMove(const Point& point);//鼠标移动时发生
		virtual void OnMouseLeave();//鼠标离开时发生
		virtual void OnMouseWheel(short zDelta, const Point& point);//鼠标滚动时发生
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//鼠标双击是发生
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//鼠标按下时发生
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//鼠标弹起时发生
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//鼠标单击并弹起时发生
		virtual void Rending(HDC winDC, const Rect& rePaint);//渲染函数 渲染前进行
		virtual void OnPaint(PaintEventArgs& arg);//渲染中
		virtual void OnLocation(const Point& point);//位置发生改变时发生
		virtual void OnSize(const Size& sz);//大小发生改变时发生
		virtual void OnRect(const Rect& rect);//当窗口矩形区域发生改变时发生
		virtual void OnClose(bool& bClose);//当窗口收到WM_CLOSE消息时发生 
		virtual void OnDestroy();//当窗口销毁时发生
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);//字符消息
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);//键盘按下
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//键盘抬起
		virtual void OnKillFocus(HWND hWnd);//失去焦点时发生
		//鼠标 键盘 重绘 会进入此函数,返回true将不再派发给sender控件处理 注意:尽量不要在此函数内部删除自身控件(鼠标和键盘事件内可以删除)
		virtual bool OnNotify(Control* sender, EventArgs& args);
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//处理消息队列的
	public:
		Control* MainLayout = NULL;//布局
		bool Zoom = true;//是否支持缩放
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();
		Control* FindControl(const Point clientPoint, Point* outPoint);//根据坐标获取控件
		Control* FindControl(const EString& objectName);//使用id寻找控件
		const HWND& Hwnd();//获取窗口句柄
		const Rect& GetWindowRect();//获取窗口基于显示器的矩形
		const Rect& GetClientRect();//获取客户区矩形
		void SetSize(const Size& size);//设置窗口size
		void SetLocation(const Point& pt);//设置窗口位置
		void SetRect(const Rect& rect);//设置窗口矩形
		void SetMiniSize(const Size& size);//设置窗口最小size
		void SetMaxSize(const Size& size);//设置窗口最大size
		void SetIcon(short id);//设置窗口ico
		void SetIcon(HICON icon);//设置窗口ico
		void SetLayout(EzUI::Control* layout);//设置窗口主布局
		void SetText(const EString& text);//设置窗口标题
		void SetTopMost(bool top);//设置与取消窗口置顶
		bool IsTopMost();//窗口是否置顶
		virtual void Show(int cmdShow = SW_SHOW);//操作窗口的显示
		virtual void Hide();//隐藏窗口
		void ShowNormal();//正常显示窗口
		void Close(int exitCode = 0);//关闭窗口 exitCode为退出代码
		int ShowModal(bool wait = true);//参数 wait 是否阻塞
		void ShowMinimized();//最小化窗口
		void ShowMaximized();//最大化窗口
		bool IsVisible();//窗口是否显示
		void SetVisible(bool flag);
		void Invalidate();//使区域无效(延迟刷新)
		void Refresh();//立即更新所有无效区域(立即刷新)

	};
};