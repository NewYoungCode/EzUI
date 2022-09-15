#pragma once
#include "EzUI.h"
#include "Painter.h" 
namespace EzUI {
	enum  Event :int {
		OnMouseWheel = 1,
		OnMouseEnter = 2,
		OnMouseMove = 4,
		OnMouseLeave = 8,
		OnMouseClick = 16,
		OnMouseDoubleClick = 32,
		OnMouseDown = 64,
		OnMouseUp = 128,
		OnKeyDown = 256,
		OnKeyUp = 512,
		OnPaint = 1024,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnKillFocus = 2048
	};
	enum class ControlState {
		None,
		Hover,
		Active,
		Disable
	};
	enum class DockStyle {
		// ժҪ:
		//δ����
		None,
		// ժҪ:
		//�ڸ��ؼ��� ���ұ���
		Horizontal,
		// ժҪ:
		//�ڸ��ؼ��� ���±���
		Vertical,
		// ժҪ:
		// �����������ؼ�
		Fill
	};
	enum class MouseButton {
		// ժҪ: 
		  //     δ��������갴ť��
		None = 0,
		//
		// ժҪ: 
		//     �����ť�����¡�
		Left = 1048576,
		//
		// ժҪ: 
		//     ����Ұ�ť�����¡�
		Right = 2097152,
		//
		// ժҪ: 
		//     ����а�ť�����¡�
		Middle = 4194304,
		//
		// ժҪ: 
		//     �� 1 �� XButton �����¡�
		XButton1 = 8388608,
		//
		// ժҪ: 
		//     �� 2 �� XButton �����¡�
		XButton2 = 16777216,
	};
	enum class Cursor :ULONG_PTR
	{
		None = 0,//δָ��
		APPSTARTING = (ULONG_PTR)IDC_APPSTARTING,// ��׼�ļ�ͷ��Сɳ©
		ARROW = (ULONG_PTR)IDC_ARROW,// ��׼�ļ�ͷ
		CROSS = (ULONG_PTR)IDC_CROSS,// ʮ�ֹ��
		HAND = (ULONG_PTR)IDC_HAND,// Windows 98/Me, Windows 2000/XP: Hand
		HELP = (ULONG_PTR)IDC_HELP,// ��׼�ļ�ͷ���ʺ�
		IBEAM = (ULONG_PTR)IDC_IBEAM,// ���ֹ��
		ICON = (ULONG_PTR)IDC_ICON,// Obsolete for applications marked version 4.0 or later.
		NO = (ULONG_PTR)IDC_NO,// ��ֹȦ
		SIZE = (ULONG_PTR)IDC_SIZE,// Obsolete for applications marked version 4.0 or later. Use SIZEALL.
		SIZEALL = (ULONG_PTR)IDC_SIZEALL,// �����ͷָ�򶫡������ϡ���
		SIZENESW = (ULONG_PTR)IDC_SIZENESW,// ˫��ͷָ�򶫱�������
		SIZENS = (ULONG_PTR)IDC_SIZENS, // ˫��ͷָ���ϱ�
		SIZENWSE = (ULONG_PTR)IDC_SIZENWSE,// ˫��ͷָ�������Ͷ���
		SIZEWE = (ULONG_PTR)IDC_SIZEWE,// ˫��ͷָ����
		UPARROW = (ULONG_PTR)IDC_UPARROW,// ��ֱ��ͷ
		WAIT = (ULONG_PTR)IDC_WAIT// ɳ©��Windows7�»���ʾΪѡ���ԲȦ��ʾ�ȴ�
	};
	// ժҪ: 
	//�����¼�
	struct EventArgs {
		Event EventType;
	};
	// ժҪ: 
	//Ϊ����¼��ṩ��������
	struct MouseEventArgs :public EventArgs {
	public:
		MouseButton Button;
		short Delta;
		Point Location;
	public:
		MouseEventArgs() {}
		virtual ~MouseEventArgs() {}
		MouseEventArgs(const Event& eventType, const Point& location = Point(0, 0), const MouseButton& mouseButton = MouseButton::None, const short& delta = 0) {
			this->EventType = eventType;
			this->Button = mouseButton;
			this->Delta = delta;
			this->Location = location;
		}
	};
	//�ؼ���ʽ
	class UI_EXPORT ControlStyle {
	public:
		//UI_Float Opacity;//���岻͸����
		UI_Int Radius;//Բ��ϵ��
		UI_Int BorderLeft;//��߱߿�
		UI_Int BorderTop;//�����߿�
		UI_Int BorderRight;//�ұ߱߿�
		UI_Int BorderBottom;//�ײ��߿�
		Color BorderColor;//�߿���ɫ
		Color BackgroundColor;//������ɫ
		HImage BackgroundImage;//����ͼƬ
		HImage ForeImage;//ǰ��ͼƬ

		EString FontFamily;//��������   ���м̳���
		UI_Int FontSize;//�����С       ���м̳���
		Color ForeColor;//ǰ����ɫ      ���м̳���
	private:
		void operator=(const ControlStyle& right) {} //��ֱֹ�Ӹ�ֵ ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
		ControlStyle(const ControlStyle& right) {} //��ֹ���� ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void SetBorder(const Color& color, int width);
		bool IsValid();
		void SetStyleSheet(const EString& styleStr);
		void SetStyle(const EString& key, const EString& value);
	};
	//�ؼ���Ϊ
	enum class ControlAction {
		None,
		MoveWindow,//�ƶ�����
		Mini,//��С��
		Max,//���|�ָ�
		Close//�ر�
	};

#define UIFunc std::function
#define EventMouseMove  UIFunc<void(Control*,const Point&)>  //�ƶ��¼�
#define EventMouseEnter  UIFunc<void( Control*, const Point&)>//�����¼�
#define EventMouseWheel   UIFunc<void( Control*, short, const Point&)>//�����¼�
#define EventMouseLeave  UIFunc<void( Control*)>//����뿪�¼�
#define EventMouseDown  UIFunc<void( Control*, MouseButton, const Point&)> //��갴���¼�
#define EventMouseUp  UIFunc<void ( Control*, MouseButton, const Point&)>//���̧��
#define EventMouseClick   UIFunc<void( Control*, MouseButton, const Point&)>//��굥��
#define EventMouseDoubleClick   UIFunc<void( Control*, MouseButton, const Point&)>//���˫��
#define EventScrollRolling   UIFunc<void(int, int)>//�����������¼�

	typedef std::map<EString, EString> Attributes;
	typedef std::map<EString, EString>::iterator AttributeIterator;
	class Control;
	class ScrollBar;
	class Spacer;
	typedef std::list<Control*> Controls;
	typedef std::list<Control*>::iterator ControlIterator;

	//����ϵ
	class UI_EXPORT IRect {
	protected:
		Rect _rect;
		int _fixedWidth = 0;
		int _fixedHeight = 0;
	public:
		DockStyle Dock = DockStyle::None;//dock��ʽ
	public:
		const Rect ClipRect;//�ؼ��ڴ����еĿɼ�����
		const int& X();
		const int& Y();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//�ƶ�����븸�ؼ���λ��
		void SetSize(const Size& size); //���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetWidth(const int& width);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		void SetHeight(const int& height);//���ػ�ؼ�ʱ��������ʹ�� Ӱ������(�����SetRect����)
		const int& Width();
		const int& Height();
		void SetFixedWidth(const int& fixedWidth);//���þ��Կ��
		void SetFixedHeight(const int& fixedHeight);//���þ��Ը߶�
		const int& GetFixedWidth();//��ȡ���Կ��
		const int& GetFixedHeight();//��ȡ���Ը߶�
		const Rect& GetRect();//��ȡ����븸�ؼ�����
		virtual void OnLayout(const Size& parentRect, bool instantly = true);//���ؼ���С�ı��¼�  instantly������Ч
	public:
		virtual void ComputeClipRect() = 0;//������ڸ��ؼ��Ĳü�����
		virtual Rect GetClientRect() = 0;//��ȡ���ڿͻ��˵ľ���
		virtual void SetRect(const Rect& rect, bool rePaint = false) = 0;//������Ը��ؼ�����
	public:
		virtual ~IRect() {};
	};
	//������ϵ
	class UI_EXPORT IMouseKeyBoard {
	protected:
		Tuple<LPTSTR> _LastCursor;
	public:
		int MousePassThrough = 0;
		Cursor Cursor = Cursor::None;//�����ʽ
	public:
		//����¼����
		EventMouseMove MouseMove;//�ƶ��¼�
		EventMouseEnter MouseEnter;//�����¼�
		EventMouseWheel MouseWheel;//�����¼�
		EventMouseLeave MouseLeave;//����뿪�¼�
		EventMouseDown MouseDown;//��갴���¼�
		EventMouseUp MouseUp;//���̧��
		EventMouseClick MouseClick;//��굥��
		EventMouseDoubleClick MouseDoubleClick;//���˫��
	public:
		virtual ~IMouseKeyBoard() {};
		bool CheckEventPassThrough(Event eventType);
	};

	class UI_EXPORT  IScroll {
	public:
		int Margin = 0;//�����ˮƽ�б��Ǿ��� �ұ߾� ,��ֱ�����������±߾�
		std::map<Control*, int> LocationX;
		std::map<Control*, int> LocationY;
		virtual ~IScroll() {};
	};
	class UI_EXPORT IScrollBar {
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual  int RollingCurrent() = 0;
		virtual int RollingTotal() = 0;//
		EventScrollRolling Rolling = NULL;
		virtual ~IScrollBar() {};
	};

	class  UI_EXPORT IControl {
	protected:
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//����Ƿ��Ѿ�����
		bool _hasTimer = false;
		Attributes _attrs;
	public:
		UINT_PTR Tag = NULL;
		HWND _hWnd = NULL;//if son is control,the hwnd is parent window handle
	public:
		IControl();
		virtual ~IControl();
	public:
		virtual void OnMouseMove(const Point& point) = 0;
		virtual void OnMouseLeave() = 0;
		virtual void OnMouseWheel(short zDelta, const Point& point) = 0;
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point) = 0;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point) = 0;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point) = 0;
		virtual void OnSize(const Size& size) = 0;
		virtual void OnLoad() = 0;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) = 0;
		virtual void OnKeyDown(WPARAM wParam) = 0;
		virtual void OnTimer();//��ʱ������
	public:
		virtual void SetStyleSheet(const EString& styleStr);//����style
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//��������
		virtual const EString GetAttribute(const EString& attrName);//��ȡ����
		virtual UINT_PTR SetTimer(size_t interval);
		virtual void KillTimer();
	};

};