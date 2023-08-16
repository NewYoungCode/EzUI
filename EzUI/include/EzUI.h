#pragma once
#include "UIDef.h"
#include "EString.h"
#include "unzip.h"
#include "RenderType.h"
#include "Direct2DRender.h"

namespace EzUI {
	struct MonitorInfo;
	class EventArgs;
	class ControlStyle;
	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	extern WCHAR WindowClassName[];
	//ȫ����Դ���
	extern UI_EXPORT HZIP HZipResource;//zip�ļ��е�ȫ����Դ���
	extern UI_EXPORT HGLOBAL HVSResource;//vs�е���Դ�ļ����
	class UI_EXPORT Ziper {
		HZIP _hZip = NULL;
		size_t _count = 0;
		int _numitems = 0;
		ZIPENTRY _ze{ 0 };
	public:
		Ziper(const EString& fileName, const EString& password = "");
		Ziper(void* pData, size_t len, const EString& password = "");
		size_t GetCount();
		bool Find(const EString& fileName, ZIPENTRY* outZe);
		void UnZip(const ZIPENTRY& ze, void** pData);
		void UnZip(std::function<bool(int index, const EString& fileName, void* pData, size_t len, DWORD fileAttribute)> callback);
		virtual ~Ziper();
	};
	//
	extern const std::list<EzUI::MonitorInfo> MonitorInfos;
	//��ѹ�ļ�
	//extern UI_EXPORT void UnZip(const EString& zipFileName, const EString& outPath, const EString& password = "", std::function<void(int index, int fileCount)> callback = NULL);
	//�ӻ�ȡ�ļ���Դ
	extern UI_EXPORT bool GetResource(const EString& fileName, std::string* outData);
	//��ȡ��ǰ���м���������Ϣ
	extern size_t GetMonitors(std::list<MonitorInfo>* outMonitorInfo);
	//��ȡ�û���ǰ���ڲ����ĵ���ʾ��
	extern bool GetMontior(MonitorInfo* outInfo);
	extern HCURSOR LoadCursor(Cursor cursorType);
	extern HCURSOR LoadCursor(const EString& fileName);//��Ҫ�ͷ�
	extern void FreeCursor(HCURSOR hCursor);

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
				r = std::stoi(rgbList.at(0));
				g = std::stoi(rgbList.at(1));
				b = std::stoi(rgbList.at(2));
				unsigned char a = 255;
				if (colorStr.find("rgba") == 0) {
					a = std::stoi(rgbList.at(3));
				}
				return Color(a, r, g, b);
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

#if USED_DIRECT2D
	class Image :public DXImage {
	public:
		virtual ~Image() {}
		Image(UINT width, UINT height) :DXImage(width, height) {}
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const void* data, size_t dataCount) :DXImage(data, dataCount) {}
	public:
		static Image* FromFile(const EString& fileOrRes) {
			//����Դ�л�ȡ
			std::string data;
			GetResource(fileOrRes, &data);
			return new Image(data.c_str(), data.size());
		}
	};
#endif
	// �������ڱ�����ʾ����Ϣ�Ľṹ��
	struct MonitorInfo {
		//��ʾ����λ�� ����ʾ����Y����ܳ��ָ������ߴ���0��ʱ�������ʾ��������������ʾ���Ǵ�λ��(����ʾ��û��ƽ��);
		EzUI::Rect Rect;//�߼����
		EzUI::Rect WorkRect;//��������
		Size Physical;//��ʾ��������
		float Scale = 1.0f;//��ʾ�����ű���
		float FPS = 60;//��ʾ��֡��
		bool Primary = false;//�Ƿ�Ϊ����ʾ��
	};
	struct WindowData {
		void* Window = NULL;//������ʵ��
		float Scale = 1.0f;//������
		Control* FocusControl = NULL;//���н���Ŀؼ�
		Control* InputControl = NULL;//�����
		size_t PaintCount = 0;
		bool Debug = false;//�Ƿ���debugģʽ
		HWND HANDLE = NULL;//���ھ��
		std::function<void(void*)> InvalidateRect = NULL;//ʹһ��������Ч
		std::function<void()> UpdateWindow = NULL;//��������ȫ����Ч����
		std::function<bool(Control*, EventArgs&)> Notify = NULL;//
		std::function<void(Control*)> RemoveControl = NULL;//��տؼ���ǵȵ�...
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
			beg_t = end_t;
			return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		}
	};


	enum class LayoutState :byte {
		None = 1, //��״̬ (���貼��)
		Pend = 2,//������
		Layouting = 4//������
	};
	enum Event :int {
		None = 1,
		OnMouseWheel = 2,
		OnMouseEnter = 4,
		OnMouseMove = 8,
		OnMouseLeave = 16,
		OnMouseClick = 32,
		OnMouseDoubleClick = 64,
		OnMouseDown = 128,
		OnMouseUp = 256,
		OnKeyDown = 512,
		OnKeyUp = 1024,
		OnPaint = 2048,
		OnKillFocus = 4096,
		OnKeyChar = 8192,
		OnLocation = 16384,
		OnSize = 32768,
		OnRect = 65536,
		OnTextChange = 131072,
		OnDpiChange = 262144,
		OnScrollRoll = 524288,
		OnActive = OnMouseDown | OnMouseUp,
		OnMouseDrag = OnMouseDown | OnMouseMove,
		OnHover = OnMouseEnter | OnMouseLeave,
		OnMouseEvent = OnMouseWheel | OnMouseEnter | OnMouseMove | OnMouseLeave | OnMouseClick | OnMouseDoubleClick | OnMouseDown | OnMouseUp,
		OnKeyBoardEvent = OnKeyDown | OnKeyUp | OnKeyChar
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
		Checked,//ѡ��״̬
		Hover,//�������
		Active//��갴ס
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
		Event EventType = Event::None;
		EventArgs(const Event& eventType) {
			this->EventType = eventType;
		}
		virtual ~EventArgs() {};
	};
	//Ϊ����¼��ṩ��������
	class MouseEventArgs :public EventArgs {
	public:
		MouseButton Button = MouseButton::None;
		int RollCount = 0;
		short ZDelta = 0;
		Point Location;
	public:
		virtual ~MouseEventArgs() {}
		MouseEventArgs(const Event& eventType, const Point& location = Point(0, 0), const MouseButton& mouseButton = MouseButton::None, const short& zdelta = 0, int rollCount = 0) :EventArgs(eventType) {
			this->Button = mouseButton;
			this->ZDelta = zdelta;
			this->Location = location;
			this->RollCount = rollCount;
		}
	};
	//�����������¼�
	class ScrollRollEventArgs :public EventArgs {
	public:
		int Pos = 0;
		int Total = 0;
		int Speed = 0;
		short ZDelta = 0;
		//�������� һ���Ϊ���� Event::OnMouseDrag Event::OnMouseWheel ����
		Event RollType = Event::None;
	public:
		ScrollRollEventArgs(Event rollType = Event::None) :EventArgs(Event::OnScrollRoll), RollType(rollType) {
		}
		virtual ~ScrollRollEventArgs() {}
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
		KeyboardEventArgs(const Event& eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType) {
			this->wParam = wParam;
			this->lParam = lParam;
		}
		virtual ~KeyboardEventArgs() {}
	};
	//ʧȥ����
	class KillFocusEventArgs :public EventArgs {
	public:
		Control* Control;
		KillFocusEventArgs(EzUI::Control* ctl) :EventArgs(Event::OnKillFocus) {
			this->Control = ctl;
		}
		virtual ~KillFocusEventArgs() {}
	};
	//���귢���ı�
	class LocationEventArgs :public EventArgs {
	public:
		const EzUI::Point& Location;
		LocationEventArgs(const EzUI::Point& location) :EventArgs(Event::OnLocation), Location(location) {}
		virtual ~LocationEventArgs() {}
	};
	//��С�����ı�
	class SizeEventArgs :public EventArgs {
	public:
		const EzUI::Size& Size;
		SizeEventArgs(const EzUI::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//���η����ı�
	class RectEventArgs :public EventArgs {
	public:
		const EzUI::Rect& Rect;
		RectEventArgs(const EzUI::Rect& rect) :EventArgs(Event::OnRect), Rect(rect) {}
		virtual ~RectEventArgs() {}
	};
	class DpiChangeEventArgs :public EventArgs {
	public:
		float Scale = 1.0;
		DpiChangeEventArgs(const float& scale) :EventArgs(Event::OnDpiChange), Scale(scale) {}

	};
	// Ϊ OnPaint �¼��ṩ���ݡ�
	class PaintEventArgs :public EventArgs {
	public:
		std::list<Point> OffSetPoint;//���ڼ�¼ÿ�λ��ƿؼ���ƫ��λ��
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::DXRender& Graphics;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		PaintEventArgs(EzUI::DXRender& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
	};
	// Ϊ�ؼ���ʽ�ṩ���ݡ�
	class UI_EXPORT ControlStyle {
	public:
		EzUI::Border Border;//�߿���Ϣ
		//UI_Float Opacity;//���岻͸����
		Color BackColor = 0;//������ɫ
		Image* BackImage = NULL;//����ͼƬ ���ָ����ͼƬ��ɾ�� ����뽫������
		Image* ForeImage = NULL;//ǰ��ͼƬ ���ָ����ͼƬ��ɾ�� ����뽫������
		std::wstring FontFamily;//�������� ���м̳���
		int FontSize = 0;//�����С ���м̳���
		Color ForeColor;//ǰ����ɫ  ���м̳���
		HCURSOR Cursor = NULL;//�����ʽ
	private:
		void operator=(const ControlStyle& right) {} //��ֱֹ�Ӹ�ֵ ��Ϊ�����ᵼ�� Colorִ�п���ʹ��Color��ò��Ϸ�����Ч
		ControlStyle(const ControlStyle& right) {} //��ֹ���� 
	public:
		ControlStyle() {}
		virtual ~ControlStyle() {}
		void Scale(const float& scale);
		void SetStyleSheet(const EString& styleStr, const std::function<void(Image*)>& callback = NULL);
		void SetStyle(const EString& key, const EString& value, const std::function<void(Image*)>& callback = NULL);
	};

	class UI_EXPORT IControl {
	private:
		std::map<EString, EString> _attrs;
	public:
		void* Tag = NULL;
	public:
		IControl();
		virtual ~IControl();
	public:
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//��������
		virtual EString GetAttribute(const EString& attrName);//��ȡ����
	};

};