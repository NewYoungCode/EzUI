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
		Control* FindControl(const Point clientPoint, Point* outPoint);//���������ȡ�ؼ�
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
		const HWND& Hwnd();//��ȡ���ھ��
		const Rect& GetWindowRect();//��ȡ���ڻ�����ʾ���ľ���
		const Rect& GetClientRect();//��ȡ�ͻ�������
		void SetSize(const Size& size);
		void SetLocation(const Point& pt);
		void SetRect(const Rect& rect);
		void SetMiniSize(const Size& size);
		void SetMaxSize(const Size& size);
		void SetIcon(short id);//���ô���ico
		void SetIcon(HICON icon);//���ô���ico
		void SetLayout(EzUI::Control* layout);//���ô���������
		void SetText(const EString& text);//���ô��ڱ���
		void SetTopMost(bool top);//������ȡ�������ö�
		bool IsTopMost();//�����Ƿ��ö�
		virtual void Show(int cmdShow = SW_SHOW);
		virtual void Hide();
		void ShowNormal();
		void Close(int exitCode = 0);
		int ShowModal(bool wait = true);//���� wait �Ƿ�����
		void ShowMinimized();//��С������
		void ShowMaximized();//��󻯴���
		bool IsVisible();
		void SetVisible(bool flag);
		void Invalidate();//ʹ������Ч(�ӳ�ˢ��)
		void Refresh();//��������������Ч����(����ˢ��)

	};
};