#pragma once
#include "EzUI.h"
#include "Painter.h"
#include "IControl.h"
#include "Control.h"
#include "Layout.h"
#include "Layout.h"
#include "Edit.h"
#include "Container.h"
class UI_EXPORT Window :public IControl
{
private:
	Size _lastSize;//上一次客户端大小的信息
	Point _lastPoint;//上一次移动的坐标
protected:
	Layout *_layout = NULL;//布局
	Rect _rect;//基于桌面的坐标
	Rect _rectClient;//客户绘图区域
	bool _mouseDown = false;//鼠标是否已经按下
	UserMessageProc _userMessageProc = NULL;
	Control *_focusControl = NULL;//具有焦点的控件
	Control *_inputControl = NULL;//输入框
	HWND _hWnd = NULL;//当前窗口句柄
private:
	Window(const Window&) {};
	bool IsInWindow(Control & pControl, Control & it);//控件是否在窗口的可见区域
	Control*  FindControl(const Point & clientPoint, Point&outPoint);//根据坐标获取控件
	void EmptyControl(Controls * controls);
protected:
	void MoveWindow();//鼠标按下移动窗口
	virtual void OnMouseMove(const Point&point);
	virtual void OnMouseLeave();
	virtual void OnMouseWheel(short zDelta, const Point&point);
	virtual void OnMouseDoubleClick(MouseButton mbtn, const Point&point);
	virtual void OnMouseDown(MouseButton mbtn, const Point&point);
	virtual void OnMouseUp(MouseButton mbtn, const Point&point);
	virtual void OnPaint(HDC hdc, const Rect&rect);
	virtual void OnSize(const Size&sz);
	virtual void OnRect(const Rect&rect);
	virtual bool OnClose();
	virtual void OnDestroy();
	virtual void OnLoad();
	virtual void OnChar(WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(WPARAM wParam);
	virtual void OnMove(const Point &point);
	virtual bool OnNotify(Control * sender, EventArgs * args);//返回true将不再派发给子控件处理
public:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
	virtual ~Window();
	void SetUserMessageProc(const UserMessageProc&userMessageProc);
	Control* FindControl(const EString& objectName);
	HWND Hwnd();
	Rect& GetRect();
	Rect& GetClientRect();
	void ReSize(const Size&size);
	void SetIcon(short id);
	void SetIcon(HICON icon);
	void SetLayout(Layout*layout);
	void SetText(const EString&text);
	void Show(int cmdShow = SW_SHOW);
	void ShowModal(bool wait = true);//参数 wait 是否阻塞
	void Close();
	virtual void Hide();
	bool IsVisible();
	void SetVisible(bool flag);
};
