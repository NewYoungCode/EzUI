#pragma once
#include "EzUI.h"
#include "Painter.h"
#include "IControl.h"
#include "Control.h"
#include "Layout.h"
#include "Edit.h"
namespace EzUI {

	class UI_EXPORT Window :public IControl
	{
	private:
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);
		Control* _lastDownCtl = NULL;
		Point* _mouseDbClick = NULL;
		Size _lastSize;//��һ�οͻ��˴�С����Ϣ
		Point _lastPoint;//��һ���ƶ�������
		int _closeCode = 0;
		HWND _OwnerHwnd = NULL;
	protected:
		WindowData _winData;
		Rect _rect;//�������������
		Rect _rectClient;//�ͻ���ͼ����
		Control* _focusControl = NULL;//���н���Ŀؼ�
		Control* _inputControl = NULL;//�����
	private:
		Window(const Window&) {};
		bool IsInWindow(Control& pControl, Control& it);//�ؼ��Ƿ��ڴ��ڵĿɼ�����
		Control* FindControl(const Point& clientPoint, Point& outPoint);//���������ȡ�ؼ�
		void EmptyControl(Controls* controls);
	protected:
		void MoveWindow();//��갴���ƶ�����
		virtual void OnMouseMove(const Point& point);
		virtual void OnMouseLeave();
		virtual void OnMouseWheel(short zDelta, const Point& point);
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		virtual void OnPaint(HDC hdc, const Rect& rect);
		virtual void OnSize(const Size& sz);
		virtual void OnRect(const Rect& rect);
		virtual void OnClose(bool& Cancel);
		virtual void OnDestroy();
		virtual void OnLoad();
		virtual void OnChar(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyDown(WPARAM wParam);
		virtual void OnMove(const Point& point);
		virtual bool OnNotify(Control* sender, const EventArgs& args);//����true�������ɷ����ӿؼ����� ע��:��Ҫ�ڴ˺����ڲ�ɾ������ؼ�
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Layout* MainLayout = NULL;//����
		bool Zoom = false;
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		virtual ~Window();
		Control* FindControl(const EString& objectName);
		HWND Hwnd();
		Rect& GetRect();
		Rect& GetClientRect();
		void ReSize(const Size& size);
		void SetIcon(short id);
		void SetIcon(HICON icon);
		void SetLayout(EzUI::Layout* layout);
		void SetText(const EString& text);
		virtual void Show(int cmdShow = SW_SHOW);
		int ShowModal(bool wait = true);//���� wait �Ƿ�����
		void Close(int code = 0);
		virtual void Hide();
		bool IsVisible();
		void SetVisible(bool flag);
	};
};