#pragma once
#include "UIDef.h"
#include "EString.h"
#include "unzip.h"
#include "RenderType.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"
namespace EzUI {

	//ȫ����Դ���
	extern UI_EXPORT HZIP HZipResource;//zip�ļ��е�ȫ����Դ���
	extern UI_EXPORT HGLOBAL HVSResource;//vs�е���Դ�ļ����
	//��ȡ��ǰ�߳�ID
	extern UI_EXPORT size_t GetThreadId();
	//��ȡ��ǰ�߳�ע��Ĵ�������
	extern UI_EXPORT StdString GetThisClassName();
	//��ȫ��zip��ȡ��Դ
	extern UI_EXPORT bool GetGlobalResource(const EString& fileName, std::string** outData);
	//��ȫ��zip��ȡ��Դ
	extern UI_EXPORT bool GetGlobalResource(const EString& fileName, IStream** outData);

	using RectF = RenderType::RectF;
	using Size = RenderType::Size;
	using SizeF = RenderType::SizeF;
	using Point = RenderType::Point;
	using PointF = RenderType::PointF;
	using ARGB = RenderType::ARGB;

	class EventArgs;
	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	class UI_EXPORT Rect :public RenderType::Rect {
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
	class UI_EXPORT Color :public RenderType::Color {
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
	class UI_EXPORT EBitmap {
	public:
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HDC _hdc = NULL;
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
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y);
		void Earse(const Rect& rect);//Ĩ����������
		void FillRect(const Rect& rect, const Color& color);//

		HDC& GetDC();
		virtual ~EBitmap();
	};

#if USED_GDIPLUS
	class Image :public GdiplusImage {
	public:
		Image(HBITMAP hBitmap) :GdiplusImage(hBitmap) {}
		Image(IStream* iStream) :GdiplusImage(iStream) {}

		Image(const EString& fileOrRes) :GdiplusImage((IStream*)NULL) {
			IStream* iStream = NULL;
			if (GetGlobalResource(fileOrRes, &iStream)) {
				this->CreateFormStream(iStream);
				iStream->Release();
			}
			else {
				this->CreateFromFile(fileOrRes.utf16());
			}
		}
	};
#endif
#if USED_Direct2D
	class Image :public DXImage {
	public:
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const EString& fileOrRes) {
			IStream* iStream = NULL;
			if (GetGlobalResource(fileOrRes, &iStream)) {
				this->CreateFormStream(iStream);
				iStream->Release();
			}
			else {
				this->CreateFromFile(fileOrRes.utf16());
			}
		}
	};
#endif

	template<typename T>
	class Tuple {
	private:
		T value = NULL;
	public:
		bool valid = false;
		Tuple() :valid(false) {
		}
		Tuple(const T& value) {
			this->value = value;
			valid = true;
		}
		operator T () {
			return value;
		}
		T operator->() {
			return value;
		}
	};
	template<typename T>
	class SafeObject {
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
	//�о�c#��ί�кܺ��� ���Գ�Ϯһ��
	template<class out, class...in>
	class Delegate {
		using TFunc = std::function<out(in...)>;
		std::map<size_t, TFunc> callbacks;
	public:
		auto operator+=(const TFunc& callback) {
			auto pair = std::pair<size_t, TFunc>(size_t(&callback), callback);
			callbacks.emplace(pair);
			return size_t(&callback);
		}
		void operator-=(size_t eventid) {
			callbacks.erase(eventid);
		}
		void operator()(in... args) {
			for (auto& it : callbacks) {
				it.second(std::forward<in>(args)...);
			}
		}
	};
	struct WindowData {
		void* Window = NULL;//������ʵ��
		Control* FocusControl = NULL;//���н���Ŀؼ�
		Control* InputControl = NULL;//�����
		bool Debug = false;//�Ƿ���debugģʽ
		HWND HANDLE = NULL;//���ھ��
		UIFunc<void(void*)> InvalidateRect = NULL;//ʹһ��������Ч
		UIFunc<void()> UpdateWindow = NULL;//��������ȫ����Ч����
		UIFunc<bool(Control*, EventArgs&)> Notify = NULL;//
		UIFunc<void(Control*, const std::wstring&)> SetTips = NULL;//����������ʾ����
		UIFunc<void(Control*)> DelTips = NULL;//�Ƴ�������ʾ����
		UIFunc<void(Control*)> RemoveControl = NULL;//��տؼ���ǵȵ�...
	};
	class StopWatch {
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

	extern size_t __count_onsize;

	enum class LayoutState :byte {
		None, //��״̬ (���貼��)
		Pend,//������
		Layouting//������
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
		OnKillFocus = 2048,
		OnChar = 4096,
		OnLocation = 8192,
		OnSize = 16384,
		OnTextChange = 32768,
		OnActive = OnMouseDown | OnMouseUp,
		OnHover = OnMouseEnter | OnMouseLeave
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
		None,
		//
		// ժҪ: 
		//     �����ť�����¡�
		Left,
		//
		// ժҪ: 
		//     ����Ұ�ť�����¡�
		Right,
		//
		// ժҪ: 
		//     ����а�ť�����¡�
		Middle,
		//
		// ժҪ: 
		//     �� 1 �� XButton �����¡�
		XButton1,
		//
		// ժҪ: 
		//     �� 2 �� XButton �����¡�
		XButton2
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

	//�����¼�
	class EventArgs {
	public:
		Event EventType;
		virtual ~EventArgs() {};
	};
	// ժҪ: 
	//Ϊ����¼��ṩ��������
	class MouseEventArgs :public EventArgs {
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
	// ժҪ: 
	//Ϊ�����¼��ṩ��������
	class KeyboardEventArgs :public EventArgs {
	public:
		/// <summary>
		/// һ����ָ ���̵�asciiֵ
		/// </summary>
		WPARAM wParam;
		LPARAM lParam;
		KeyboardEventArgs(const Event& eventType, WPARAM wParam, LPARAM lParam) {
			this->EventType = eventType;
			this->wParam = wParam;
			this->lParam = lParam;
		}
	};
	//���귢���ı�
	class LocationEventArgs :public EventArgs {
	public:
		EzUI::Point PrevLocation;
		EzUI::Point Location;
		LocationEventArgs() {
			this->EventType = Event::OnLocation;
		}
	};
	//��С�����ı�
	class SizeEventArgs :public EventArgs {
	public:
		EzUI::Size PrevSize;
		EzUI::Size Size;
		SizeEventArgs() {
			this->EventType = Event::OnSize;
		}
	};
	// ժҪ: 
	// Ϊ OnPaint �¼��ṩ���ݡ�
	class PaintEventArgs :public EventArgs {
	public:
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::Painter& Painter;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		PaintEventArgs(EzUI::Painter& painter) :Painter(painter) {
			EventType = Event::OnPaint;
		}
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
	typedef UIFunc<void(PaintEventArgs&)> EventPaint;//����
	typedef UIFunc<void(EString)> EventTextChange;//���ֱ���¼�

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
		ControlStyle(const ControlStyle& right) {} //��ֹ���� 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void SetBorder(const Color& color, int width);
		bool IsValid();
		void SetStyleSheet(const EString& styleStr);
		void SetStyle(const EString& key, const EString& value);
	};
	class UI_EXPORT IScroll {
	public:
		std::map<Control*, int> LocationX;
		std::map<Control*, int> LocationY;
		virtual ~IScroll() {};
	};
	class UI_EXPORT IScrollBar {
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual int RollingCurrent() = 0;
		virtual int RollingTotal() = 0;//
		virtual void OwnerSize(const Size& parentSize) = 0;
		EventScrollRolling Rolling = NULL;//�����¼�
		virtual ~IScrollBar() {};
	};
	class UI_EXPORT IControl {
	private:
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
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) = 0;
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) = 0;
	public:
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//��������
		virtual EString GetAttribute(const EString& attrName);//��ȡ����
	};

	namespace Debug {
		template<typename ...T>
		inline void Log(const EString& formatStr, T ...args) {
#ifdef DEBUGLOG
			char buf[1025]{ 0 };
			auto count = sprintf_s((buf), 1024, formatStr.c_str(), std::forward<T>(args)...);
			buf[count] = '\n';
			buf[count + 1] = NULL;
			OutputDebugStringW(EString(buf).utf16().c_str());
#endif
		}
	};
};