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
	Size _lastSize;//��һ�οͻ��˴�С����Ϣ
	Point _lastPoint;//��һ���ƶ�������
protected:
	Layout *_layout = NULL;//����
	Rect _rect;//�������������
	Rect _rectClient;//�ͻ���ͼ����
	bool _mouseDown = false;//����Ƿ��Ѿ�����
	UserMessageProc _userMessageProc = NULL;
	Control *_focusControl = NULL;//���н���Ŀؼ�
	Control *_inputControl = NULL;//�����
	HWND _hWnd = NULL;//��ǰ���ھ��
private:
	Window(const Window&) {};
	bool IsInWindow(Control & pControl, Control & it);//�ؼ��Ƿ��ڴ��ڵĿɼ�����
	Control*  FindControl(const Point & clientPoint, Point&outPoint);//���������ȡ�ؼ�
	void EmptyControl(Controls * controls);
protected:
	void MoveWindow();//��갴���ƶ�����
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
	virtual bool OnNotify(Control * sender, EventArgs * args);//����true�������ɷ����ӿؼ�����
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
	void ShowModal(bool wait = true);//���� wait �Ƿ�����
	void Close();
	virtual void Hide();
	bool IsVisible();
	void SetVisible(bool flag);
};
