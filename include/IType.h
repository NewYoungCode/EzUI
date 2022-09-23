#pragma once
#include "UIDef.h"
#include "EString.h"
#include "RenderType.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"
#pragma comment(lib, "Msimg32.lib")
namespace EzUI {
	using RectF = RenderType::RectF;
	using Size = RenderType::Size;
	using SizeF = RenderType::SizeF;
	using Point = RenderType::Point;
	using PointF = RenderType::PointF;
	using ARGB = RenderType::ARGB;

	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	struct UI_EXPORT Rect :public RenderType::Rect {
	private:
		void StringToRect(const EString& str);
	public:
		Rect() :RenderType::Rect() {};
		virtual ~Rect() {};
		Rect(IN INT x, IN INT y, IN INT width, IN INT height) :RenderType::Rect(x, y, width, height) {};
		Rect(IN const Point& location, IN const Size& size) :RenderType::Rect(location, size) {};
		Rect(const EString& rect);
		Rect& operator=(const EString& rect);
		Rect(const RECT& winRect);
	};
	struct UI_EXPORT Color :RenderType::Color {
	public:
		bool valid = false;
	private:
		void _MakeARGB(const EString& colorStr);
	public:
		Color();
		virtual ~Color() {}
		Color& operator=(const EString& colorStr);
		Color(const EString& colorStr);
		Color(IN BYTE r, IN BYTE g, IN BYTE b);
		Color(IN BYTE a, IN BYTE r, IN BYTE g, IN BYTE b);
		Color(IN ARGB argb);
		Color& operator=(const Color& Align_Right_Color);
		COLORREF COLORRE() const;
	};
	struct UI_EXPORT EBitmap {
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HDC _hdc = NULL;
		HGDIOBJ _hgdiobj = NULL;
		EBitmap(const EBitmap& hBitmap) {}
		void operator=(const EBitmap& hBitmap) {}
	public:
		HBITMAP _bitmap = NULL;
		WORD Width;
		WORD Height;
		void* point = NULL;
		BITMAPINFO bmi;
		byte biteCount = 0;
		EBitmap(WORD width, WORD height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		HDC& GetDC();
		virtual ~EBitmap();
	};

	template<typename T>
	struct Tuple {
	public:
		bool valid = false;
		T value = NULL;
		Tuple() :valid(false) {
		}
		Tuple(const T& value) {
			this->value = value;
			valid = true;
		}
		operator T () {
			return value;
		}
		T& operator->() {
			return value;
		}
	};
	template<typename T>
	struct SafeObject {
	private:
		SafeObject(const SafeObject& right) {}
		SafeObject& operator=(const SafeObject& right) {}
	public:
		T* ptr = NULL;
		SafeObject(T* _ptr) {
			ptr = _ptr;
		}
		operator T* () {
			return ptr;
		}
		~SafeObject() {
			if (ptr) delete ptr;
		}
	};

	struct WindowData {
		void* Window = NULL;//������ʵ��
		void* HoverStyles = NULL;
		void* ActiveStyles = NULL;//
		bool Debug = false;//�Ƿ���debugģʽ
		HWND HANDLE = NULL;//���ھ��
		UIFunc<void(void*)> InvalidateRect = NULL;//ʹһ��������Ч
		UIFunc<void()> UpdateWindow = NULL;//��������ȫ����Ч����
		UIFunc<bool(UINT, WPARAM, LPARAM)> Notify = NULL;//
		UIFunc<void(Control*, const std::wstring&)> SetTips = NULL;//����������ʾ����
		UIFunc<void(Control*)> DelTips = NULL;//�Ƴ�������ʾ����
		UIFunc<Cursor()> GetCursor = NULL;//��ȡ�����ʽ
		UIFunc<void(Cursor)> SetCursor = NULL;//���������ʽ
		UIFunc<void(Control*)> RemoveControl = NULL;//��տؼ���ǵȵ�...
	};
	struct StopWatch {
	private:
		std::chrono::system_clock::time_point beg_t;
	public:
		StopWatch() {
			beg_t = std::chrono::system_clock::now();    //
		}
		static time_t Now() {
			auto nowTime = std::chrono::system_clock::now();    //
			return std::chrono::system_clock::to_time_t(nowTime);
		}
		time_t ElapsedMilliseconds() {
			auto end_t = std::chrono::system_clock::now();    //
			std::chrono::duration<double> diff = end_t - beg_t;//
			return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		}
	};

	enum Event :int {
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
		OnKillFocus = 2048,
	};
	enum class ControlAction {
		None,
		MoveWindow,//�ƶ�����
		Mini,//��С��
		Max,//���|�ָ�
		Close//�ر�
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
		WAIT = (ULONG_PTR)IDC_WAIT,// ɳ©��Windows7�»���ʾΪѡ���ԲȦ��ʾ�ȴ�
		ALL = APPSTARTING | ARROW | CROSS | HAND | HELP | IBEAM | ICON | NO | SIZE | SIZEALL | SIZENESW | SIZENS | SIZENWSE | SIZEWE | UPARROW | WAIT
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

#if USED_GDIPLUS
	using Painter = GdiplusRender;
	using Image = GdiplusImage;
#endif
#if USED_Direct2D
	using Painter = Direct2DRender;
	using Image = DXImage;
#endif
	// ժҪ: 
	// Ϊ OnPaint �¼��ṩ���ݡ�
	using __Painter = Painter;
	struct PaintEventArgs {
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		Painter& Painter;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		Rect ClipRect;//��һ���ɼ��ü�����
		HWND HWnd;//�����ھ��
		PaintEventArgs(__Painter& painter) :Painter(painter) {}
		virtual ~PaintEventArgs() {}
	};


	typedef Tuple<float> UI_Float;
	typedef Tuple<int> UI_Int;
	typedef Tuple<size_t> UI_UInt;//
	typedef Tuple<Image*> HImage;
	typedef std::map<EString, EString> Attributes;//���Լ���
	typedef std::map<EString, EString>::iterator AttributeIterator;
	typedef std::list<Control*> Controls;//�ؼ�����
	typedef std::list<Control*>::iterator ControlIterator;//
	typedef UIFunc<void(Control*, const Point&)> EventMouseMove;  //�ƶ��¼�
	typedef UIFunc<void(Control*, const Point&)> EventMouseEnter;//�����¼�
	typedef UIFunc<void(Control*, short, const Point&)> EventMouseWheel;//�����¼�
	typedef UIFunc<void(Control*)> EventMouseLeave;//����뿪�¼�
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseDown; //��갴���¼�
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseUp;//���̧��
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseClick;//��굥��
	typedef UIFunc<void(Control*, MouseButton, const Point&)> EventMouseDoubleClick;//���˫��
	typedef UIFunc<void(int, int)> EventScrollRolling;//�����������¼�
	typedef UIFunc<bool(PaintEventArgs& args)> EventPaint;//����
	typedef UIFunc<bool(PaintEventArgs& args)> EventBackgroundPaint;//�������� ����ͼƬ&��ɫ
	typedef UIFunc<bool(PaintEventArgs& args)> EventForePaint;//ǰ������ ǰ��ͼƬ&����

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
	class UI_EXPORT ILayout {
	public:
		bool PendLayout = true;//�����Ƿ񱻹��� ��Addcontrol����RemoveControl��ʱ��˱�־Ϊtrue ������ResumeLayout()֮��˱�־Ϊfalse
	public:
		virtual void ResumeLayout() = 0;//���ô˺���֮���뽫 PendLayout ���ó�false
		virtual ~ILayout() {};
	};
	class UI_EXPORT IScroll {
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
	class UI_EXPORT IControl {
	protected:
		bool _load = false;
		bool _mouseIn = false;
		bool _mouseDown = false;//����Ƿ��Ѿ�����
		Attributes _attrs;
	public:
		UINT_PTR Tag = NULL;
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
		virtual EString GetAttribute(const EString& attrName);//��ȡ����
	};

};