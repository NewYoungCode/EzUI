#pragma once
#include "Control.h"

namespace EzUI {
	/// <summary>
	/// Window //������߿��ϵͳ�˵�WIN32������ʽ
	/// </summary>
	class UI_EXPORT Window :public IControl
	{
	private:
		Rect _downRect;//�ϴΰ��µ�����
		Size _miniSize;//������С�ߴ�
		Size _maxSize;//�������ߴ�
		bool _mouseIn = false;//����Ƿ�������
		bool _mouseDown = false;//����Ƿ��Ѿ�����
		std::chrono::system_clock::time_point _lastDownTime = std::chrono::system_clock::from_time_t(0);//��һ����갴�µ�ʱ��
		Size _lastSize;//��һ�οͻ��˴�С����Ϣ
		Point _lastPoint;//��һ���ƶ�������
		MouseButton _lastBtn = MouseButton::None;//��һ����갴�µİ�ť
		int _closeCode = 0;//�����ڹرյ�ʱ���˳�����
		Rect _rect;//�������������
		Rect _rectClient;//�ͻ���ͼ����
		HWND _oWnerWnd = NULL;//�������ھ��
		Control* _layout = NULL;//���ڲ���
		HWND _hWndTips = NULL;
		void InitWindow(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle);//��ʼ����
	private:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		bool IsInWindow(Control& pControl, Control& it);
	protected:
		void MoveWindow();//��갴���ƶ�����
		virtual void OnDpiChange(const float& systemScale, const Rect& newRect);//��dpi��������ʱ
		virtual void OnMouseMove(const Point& point);//����ƶ�ʱ����
		virtual void OnMouseLeave();//����뿪ʱ����
		virtual void OnMouseWheel(int zDelta, const Point& point);//����������
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//���˫���Ƿ���
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//��갴��ʱ����
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//��굯��ʱ����
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//��굥��������ʱ����
		virtual void DoPaint(HDC winDC, const Rect& rePaint);//������Ⱦ�� ��Ⱦ����
		virtual void OnPaint(PaintEventArgs& arg);//��Ⱦ��
		virtual void OnLocation(const Point& point);//λ�÷����ı�ʱ����
		virtual void OnSize(const Size& sz);//��С�����ı�ʱ����
		virtual void OnRect(const Rect& rect);//�����ھ����������ı�ʱ����
		virtual void OnClose(bool& bClose);//�������յ�WM_CLOSE��Ϣʱ���� 
		virtual void OnDestroy();//����������ʱ����
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);//�ַ���Ϣ
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);//���̰���
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//����̧��
		virtual void OnKillFocus(HWND hWnd);//ʧȥ����ʱ����
		//��� ���� �ػ� �����˺���,����true�������ɷ���sender�ؼ����� ע��:������Ҫ�ڴ˺����ڲ�ɾ�������ؼ�(���ͼ����¼��ڿ���ɾ��) �൱��QT��eventFilter
		//return true�������غ����Ĵ����߼� �����ʹ��
		virtual bool OnNotify(Control* sender, EventArgs& args);
	public:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//������Ϣ���е�
	public:
		bool Zoom = true;//�Ƿ�֧������
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD ExStyle = NULL);
		//ԭ������PostMessage
		template<class Func, class... Args>
		bool BeginInvoke(Func&& f, Args&& ...args) {
			if (PublicData->HANDLE == NULL) {
				return false;
			}
			std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			if (::PostMessage(PublicData->HANDLE, WM_GUI_SYSTEM, WM_GUI_BEGININVOKE, (LPARAM)func) == LRESULT(0)) {
				delete func;
				return false;
			}
			return true;
		}
		//ԭ������SendMessage
		template<class Func, class... Args>
		bool Invoke(Func&& f, Args&& ...args) {
			if (PublicData->HANDLE == NULL) {
				return false;
			}
			std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			if (::SendMessage(PublicData->HANDLE, WM_GUI_SYSTEM, WM_GUI_INVOKE, (LPARAM)&func) == LRESULT(-1)) {
				return false;
			}
			return true;
		}
		virtual ~Window();
		/// <summary>
		/// �ڴ�����Ѱ�����еĿؼ�
		/// </summary>
		/// <param name="clientPoint">����������Ѱ�ҿؼ�</param>
		/// <param name="outPoint">���ؿؼ����������</param>
		/// <returns>�������еĿؼ�ָ��</returns>
		Control* FindControl(const Point clientPoint, Point* outPoint);//���������ȡ�ؼ�
		Control* FindControl(const EString& objectName);//ʹ��idѰ�ҿؼ�
		const HWND& Hwnd();//��ȡ���ھ��
		const Rect& GetWindowRect();//��ȡ���ڻ�����ʾ���ľ���
		const Rect& GetClientRect();//��ȡ�ͻ�������
		const float& GetScale();//��ȡ��ǰ����dpi����
		void SetSize(const Size& size);//���ô���size
		void SetLocation(const Point& pt);//���ô���λ��
		void SetRect(const Rect& rect);//���ô��ھ���
		void SetMiniSize(const Size& size);//���ô�����Сsize
		void SetMaxSize(const Size& size);//���ô������size
		void SetIcon(short id);//���ô���icon
		void SetIcon(HICON icon);//���ô���icon
		void SetLayout(EzUI::Control* layout);//���ô���������
		Control* GetLayout();//��ȡ����������
		void SetText(const EString& text);//���ô��ڱ���
		EString GetText();//��ȡ���ڱ���
		void SetTopMost(bool top);//������ȡ�������ö�
		bool IsTopMost();//�����Ƿ��ö�
		virtual void Show();//�������ڵ���ʾ
		void Show(int cmdShow);//
		virtual void Hide();//���ش���
		void ShowNormal();//������ʾ����
		void Close(int exitCode = 0);//�رմ��� exitCodeΪ�˳�����
		virtual int ShowModal(bool disableOnwer = true);//������
		void ShowMinimized();//��С������
		void ShowMaximized();//��󻯴���
		void ShowFullScreen();//�ô���ռ����ǰ��Ļ
		bool IsVisible();//�����Ƿ���ʾ
		void SetVisible(bool flag);
		void Invalidate();//ʹ������Ч(�ӳ�ˢ��)
		void Refresh();//��������������Ч����(����ˢ��)
		void CenterToScreen();//���е���Ļ
		void CenterToWindow(HWND wnd = NULL);//�ο�ĳ�����ڽ��о���
	};
};