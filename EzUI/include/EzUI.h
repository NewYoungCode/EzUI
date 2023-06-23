#pragma once
#include "UIDef.h"
#include "EString.h"
#include "unzip.h"
#include "RenderType.h"
#include "Direct2DRender.h"

namespace EzUI {
	extern WCHAR WindowClassName[];
	//ȫ����Դ���
	extern UI_EXPORT HZIP HZipResource;//zip�ļ��е�ȫ����Դ���
	extern UI_EXPORT HGLOBAL HVSResource;//vs�е���Դ�ļ����
	//��ȡ��ǰ�߳�ID
	extern UI_EXPORT size_t GetThreadId();
	//�ӻ�ȡ�ļ���Դ
	extern UI_EXPORT bool GetResource(const EString& fileName, std::string* outData);

	class EventArgs;
	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	namespace Convert {
		inline Rect StringToRect(const EString& str) {
			auto rectStr = str.Split(",");
			Rect rect;
			if (str.empty()) {
				return rect;//���ûд��������
			}
			rect.X = std::stoi(rectStr.at(0));
			rect.Y = std::stoi(rectStr.at(1));
			rect.Width = std::stoi(rectStr.at(2));
			rect.Height = std::stoi(rectStr.at(3));
			return rect;
		}
		inline Color StringToColor(const EString& colorStr) {
			if (colorStr.find("#") == 0) { //"#4e6ef2"
				auto rStr = colorStr.substr(1, 2);
				auto gStr = colorStr.substr(3, 2);
				auto bStr = colorStr.substr(5, 2);
				unsigned int r, g, b;
				sscanf_s(rStr.c_str(), "%x", &r);
				sscanf_s(gStr.c_str(), "%x", &g);
				sscanf_s(bStr.c_str(), "%x", &b);
				//Argb = MakeARGB(255, r, g, b);
				return Color(255, r, g, b);
			}
			if (colorStr.find("rgb") == 0) { //"rgb(255,100,2,3)"
				int pos1 = colorStr.find("(");
				int pos2 = colorStr.rfind(")");
				EString rgbStr = colorStr.substr(pos1 + 1, pos2 - pos1 - 1);
				auto rgbList = rgbStr.Split(",");
				unsigned char r, g, b;
				float a = rgbList.size() == 3 ? 1 : std::stof(rgbList.at(3));//͸���ٷֱ� 0~1
				r = std::stoi(rgbList.at(0));
				g = std::stoi(rgbList.at(1));
				b = std::stoi(rgbList.at(2));
				//Argb = MakeARGB((byte)(255 * (a > 1 ? 1 : a)), r, g, b);
				return Color((byte)(255 * (a > 1 ? 1 : a)), r, g, b);
			}
			return Color();
		}
	}

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

#if USED_Direct2D
	class Image :public DXImage {
	public:
		virtual ~Image() {}
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const EString& fileOrRes) {
			//����Դ�л�ȡ
			std::string data;
			GetResource(fileOrRes, &data);
			IStream* stream = SHCreateMemStream((BYTE*)data.c_str(), data.size());
			if (stream) {
				this->CreateFormStream(stream);
				stream->Release();
			}
		}
	};
#endif

	struct WindowData {
		void* Window = NULL;//������ʵ��
		Control* FocusControl = NULL;//���н���Ŀؼ�
		Control* InputControl = NULL;//�����
		size_t PaintCount = 0;
		bool Debug = false;//�Ƿ���debugģʽ
		HWND HANDLE = NULL;//���ھ��
		std::function<void(void*)> InvalidateRect = NULL;//ʹһ��������Ч
		std::function<void()> UpdateWindow = NULL;//��������ȫ����Ч����
		std::function<bool(Control*, EventArgs&)> Notify = NULL;//
		std::function<void(Control*)> RemoveControl = NULL;//��տؼ���ǵȵ�...
		std::function<bool(Control*)> Contains = NULL;
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
		OnKeyChar = 4096,
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
		None,//��״̬ ����ʹ��_nowStyle������ʽ
		Static,//��̬
		Hover,//�������
		Active,//��갴ס
		Disable//����
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
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::DXRender& Graphics;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		PaintEventArgs(EzUI::DXRender& _painter) :Graphics(_painter) {
			EventType = Event::OnPaint;
		}
	};

	typedef std::map<EString, EString> Attributes;//���Լ���
	typedef std::map<EString, EString>::iterator AttributeIterator;
	typedef std::list<Control*> Controls;//�ؼ�����
	typedef std::list<Control*>::iterator ControlIterator;//
	typedef std::function<void(Control*, const Point&)> EventMouseMove;  //�ƶ��¼�
	typedef std::function<void(Control*, const Point&)> EventMouseEnter;//�����¼�
	typedef std::function<void(Control*, short, const Point&)> EventMouseWheel;//�����¼�
	typedef std::function<void(Control*)> EventMouseLeave;//����뿪�¼�
	typedef std::function<void(Control*, MouseButton, const Point&)> EventMouseDown; //��갴���¼�
	typedef std::function<void(Control*, MouseButton, const Point&)> EventMouseUp;//���̧��
	typedef std::function<void(Control*, MouseButton, const Point&)> EventMouseClick;//��굥��
	typedef std::function<void(Control*, MouseButton, const Point&)> EventMouseDoubleClick;//���˫��
	typedef std::function<void(Control*, WPARAM, LPARAM)> EventKeyChar;//�����¼�(WM_CHAR)
	typedef std::function<void(Control*, WPARAM, LPARAM)> EventKeyDown;//���̰����¼�
	typedef std::function<void(Control*, WPARAM, LPARAM)> EventKeyUp;//���̵����¼�
	typedef std::function<void(int, int)> EventScrollRolling;//�����������¼�
	typedef std::function<void(PaintEventArgs&)> EventPaint;//����
	typedef std::function<void(EString)> EventTextChange;//���ֱ���¼�

	class UI_EXPORT ControlStyle {
	public:
		//UI_Float Opacity;//���岻͸����
		int Radius = 0;//Բ��ϵ��
		int BorderLeft = 0;//��߱߿�
		int BorderTop = 0;//�����߿�
		int BorderRight = 0;//�ұ߱߿�
		int BorderBottom = 0;//�ײ��߿�
		Color BorderColor = 0;//�߿���ɫ
		Color BackgroundColor = 0;//������ɫ
		Image* BackgroundImage = NULL;//����ͼƬ ���ָ����ͼƬ��ɾ�� ����뽫������
		Image* ForeImage = NULL;//ǰ��ͼƬ ���ָ����ͼƬ��ɾ�� ����뽫������
		EString FontFamily;//�������� ���м̳���
		int FontSize = 0;//�����С ���м̳���
		Color ForeColor;//ǰ����ɫ  ���м̳���
	private:
		void operator=(const ControlStyle& right) {} //��ֱֹ�Ӹ�ֵ ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
		ControlStyle(const ControlStyle& right) {} //��ֹ���� 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void SetBorder(const Color& color, int width);
		void SetStyleSheet(const EString& styleStr, const std::function<void(Image*)>& callback = NULL);
		void SetStyle(const EString& key, const EString& value, const std::function<void(Image*)>& callback = NULL);
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
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam) = 0;
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
			buf[count + 1] = 0;
			auto wstr = EString(buf).utf16();
			OutputDebugStringW(wstr.c_str());
#endif
		}
	};
};