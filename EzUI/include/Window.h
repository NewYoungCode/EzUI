#pragma once
#include "Control.h"
#include "TextBox.h"

namespace EzUI {

	class UI_EXPORT Window :public IControl
	{
	private:
		Rect _downRect;
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//����Ƿ��Ѿ�����
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);
		Size _lastSize;//��һ�οͻ��˴�С����Ϣ
		Point _lastPoint;//��һ���ƶ�������
		MouseButton _lastBtn = MouseButton::None;
		int _closeCode = 0;
		HWND _OwnerHwnd = NULL;
		HWND _hWnd = NULL;
		Rect _rect;//�������������
		Rect _rectClient;//�ͻ���ͼ����
		void InitData(const DWORD& ExStyle);
	public:
		WindowData PublicData;//�洢��������
	private:
		Window(const Window&) {};
		bool IsInWindow(Control& pControl, Control& it);
		bool FindControl(Control* nodeCtl, Control* findControl);
		//�ؼ��Ƿ��ڴ��ڵĿɼ�����
	protected:
		LRESULT ZoomWindow(const LPARAM& lParam);//���Ŵ���
		void MoveWindow();//��갴���ƶ�����
		virtual void OnMouseMove(const Point& point);
		virtual void OnMouseLeave();
		virtual void OnMouseWheel(short zDelta, const Point& point);
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);
		virtual void Rending(HDC winDC, const Rect& rePaint);
		virtual void OnPaint(PaintEventArgs& arg);
		virtual void OnSize(const Size& sz);
		virtual void OnRect(const Rect& rect);
		virtual void OnClose(bool& Cancel);
		virtual void OnDestroy();
		virtual void OnLoad();
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		virtual void OnMove(const Point& point);
		//��� ���� �ػ� �����˺���,����true�������ɷ���sender�ؼ����� ע��:������Ҫ�ڴ˺����ڲ�ɾ������ؼ�(���ͼ����¼��ڿ���ɾ��)
		virtual bool OnNotify(Control* sender, EventArgs& args);
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
		void SetSize(const Size& size);
		void SetIcon(short id);
		void SetIcon(HICON icon);
		void SetLayout(EzUI::Control* layout);
		void SetText(const EString& text);
		virtual void Show(int cmdShow = SW_SHOW);
		virtual void ShowMax();
		int ShowModal(bool wait = true);//���� wait �Ƿ�����
		void Close(int code = 0);
		virtual void Hide();
		bool IsVisible();
		void SetVisible(bool flag);
		void Invalidate();//ʹ������Ч(�ӳ�ˢ��)
		void Refresh();//��������������Ч����(����ˢ��)
		Control* FindControl(const Point clientPoint, Point* outPoint);//���������ȡ�ؼ�
	};
};