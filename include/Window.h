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
		bool _mouseDown = false;//����Ƿ��Ѿ�����
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);
		Control* _lastDownCtl = NULL;
		Point* _mouseDbClick = NULL;
		Size _lastSize;//��һ�οͻ��˴�С����Ϣ
		Point _lastPoint;//��һ���ƶ�������
		int _closeCode = 0;
		HWND _OwnerHwnd = NULL;
		HWND  _hWndTip = NULL;
		HWND _hWnd = NULL;
		Rect _rect;//�������������
		Rect _rectClient;//�ͻ���ͼ����
		Control* _focusControl = NULL;//���н���Ŀؼ�
		Control* _inputControl = NULL;//�����
		void InitData(const DWORD& ExStyle);
	public:
		WindowData PublicData;//�洢��������
	private:
		Window(const Window&) {};
		bool IsInWindow(Control& pControl, Control& it);//�ؼ��Ƿ��ڴ��ڵĿɼ�����
		Control* FindControl(const Point& clientPoint, Point& outPoint);//���������ȡ�ؼ�
	protected:
		void MoveWindow();//��갴���ƶ�����
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
		virtual bool OnNotify(Control* sender, const EventArgs& args);//����true�������ɷ����ӿؼ����� ע��:��Ҫ�ڴ˺����ڲ�ɾ������ؼ�
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Control* MainLayout = NULL;//����
		bool Zoom = true;//�Ƿ�֧������
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
		int ShowModal(bool wait = true);//���� wait �Ƿ�����
		void Close(int code = 0);
		virtual void Hide();
		bool IsVisible();
		void SetVisible(bool flag);
	};
};