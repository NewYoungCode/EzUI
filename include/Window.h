#pragma once
#include "Control.h"
#include "Layout.h"
#include "Edit.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
namespace EzUI {

	class UI_EXPORT Window :public IControl
	{
	private:
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//鼠标是否已经按下
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);
		Control* _lastDownCtl = NULL;
		Point* _mouseDbClick = NULL;
		Size _lastSize;//上一次客户端大小的信息
		Point _lastPoint;//上一次移动的坐标
		int _closeCode = 0;
		HWND _OwnerHwnd = NULL;
		HWND  _hWndTip = NULL;
		HWND _hWnd = NULL;
		Rect _rect;//基于桌面的坐标
		Rect _rectClient;//客户绘图区域
		Control* _focusControl = NULL;//具有焦点的控件
		Control* _inputControl = NULL;//输入框
		void InitData(const DWORD& ExStyle);
	public:
		WindowData PublicData;//存储公共数据
	private:
		Window(const Window&) {};
		bool IsInWindow(Control& pControl, Control& it);//控件是否在窗口的可见区域
		Control* FindControl(const Point& clientPoint, Point& outPoint);//根据坐标获取控件
	protected:
		void MoveWindow();//鼠标按下移动窗口
		virtual void OnMouseMove(const Point& point);
		virtual void OnMouseLeave();
		virtual void OnMouseWheel(short zDelta, const Point& point);
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		virtual void OnPaint(HDC hdc, const Rect& rect);
		virtual bool OnSize(const Size& sz);
		virtual void OnRect(const Rect& rect);
		virtual void OnClose(bool& Cancel);
		virtual void OnDestroy();
		virtual void OnLoad();
		virtual void OnChar(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		virtual void OnMove(const Point& point);
		virtual bool OnNotify(Control* sender, const EventArgs& args);//返回true将不再派发给子控件处理 注意:不要在此函数内部删除自身控件
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Control* MainLayout = NULL;//布局
		bool Zoom = true;//是否支持缩放
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();
		Control* FindControl(const EString& objectName);
		HWND& Hwnd();
		Rect& GetRect();
		Rect& GetClientRect();
		void ReSize(const Size& size);
		void SetIcon(short id);
		void SetIcon(HICON icon);
		void SetLayout(EzUI::Control* layout);
		void SetText(const EString& text);
		virtual void Show(int cmdShow = SW_SHOW);
		int ShowModal(bool wait = true);//参数 wait 是否阻塞
		void Close(int code = 0);
		virtual void Hide();
		bool IsVisible();
		void SetVisible(bool flag);
	};
};