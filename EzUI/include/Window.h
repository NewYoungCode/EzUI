#pragma once
#include "Control.h"
#include "TextBox.h"

namespace EzUI {

	class UI_EXPORT Window :public IControl
	{
	private:
		Rect _downRect;
		Size _miniSize;
		Size _maxSize;
		bool _load = false;
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
		Control* FindControl(const Point clientPoint, Point* outPoint);//根据坐标获取控件
		//控件是否在窗口的可见区域
	protected:
		LRESULT ZoomWindow(const LPARAM& lParam);//缩放窗口
		void MoveWindow();//鼠标按下移动窗口
		virtual void OnMouseMove(const Point& point);
		virtual void OnMouseLeave();
		virtual void OnMouseWheel(short zDelta, const Point& point);
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);
		virtual void Rending(HDC winDC, const Rect& rePaint);
		virtual void OnPaint(PaintEventArgs& arg);
		virtual void OnLocation(const Point& point);
		virtual void OnSize(const Size& sz);
		virtual void OnRect(const Rect& rect);
		virtual void OnClose(bool& Cancel);
		virtual void OnDestroy();
		virtual void OnLoad();
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		virtual void OnKillFocus(HWND hWnd);
		//鼠标 键盘 重绘 会进入此函数,返回true将不再派发给sender控件处理 注意:尽量不要在此函数内部删除自身控件(鼠标和键盘事件内可以删除)
		virtual bool OnNotify(Control* sender, EventArgs& args);
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Control* MainLayout = NULL;//布局
		bool Zoom = true;//是否支持缩放
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();
		Control* FindControl(const EString& objectName);
		const HWND& Hwnd();//获取窗口句柄
		const Rect& GetWindowRect();//获取窗口基于显示器的矩形
		const Rect& GetClientRect();//获取客户区矩形
		void SetSize(const Size& size);
		void SetLocation(const Point& pt);
		void SetRect(const Rect& rect);
		void SetMiniSize(const Size& size);
		void SetMaxSize(const Size& size);
		void SetIcon(short id);//设置窗口ico
		void SetIcon(HICON icon);//设置窗口ico
		void SetLayout(EzUI::Control* layout);//设置窗口主布局
		void SetText(const EString& text);//设置窗口标题
		void SetTopMost(bool top);//设置与取消窗口置顶
		bool IsTopMost();//窗口是否置顶
		virtual void Show(int cmdShow = SW_SHOW);
		virtual void Hide();
		void ShowNormal();
		void Close(int exitCode = 0);
		int ShowModal(bool wait = true);//参数 wait 是否阻塞
		void ShowMinimized();//最小化窗口
		void ShowMaximized();//最大化窗口
		bool IsVisible();
		void SetVisible(bool flag);
		void Invalidate();//使区域无效(延迟刷新)
		void Refresh();//立即更新所有无效区域(立即刷新)

	};
};