#pragma once
#include "UIDef.h"
#include "EString.h"
#include "unzip.h"
#include "RenderType.h"
#include "Direct2DRender.h"
#include <TimeAPI.h>

namespace EzUI {
	struct MonitorInfo;
	class IControl;
	class EventArgs;
	class ControlStyle;
	class Control;
	class Spacer;
	class ScrollBar;
	enum class Cursor :ULONG_PTR;

	namespace Base {
		//ȫ����Դ���
		extern UI_EXPORT WCHAR WindowClassName[];//��������
		extern UI_EXPORT std::mutex ResourceMtx;//��Դ��
		extern UI_EXPORT HZIP HZipResource;//zip�ļ��е�ȫ����Դ���
		extern UI_EXPORT HGLOBAL HVSResource;//vs�е���Դ�ļ����
		extern UI_EXPORT const std::list<EzUI::MonitorInfo> MonitorInfos;//���м�������Ϣ
	};

	//��ѹ�����
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
	//�������ݵ����а�
	extern UI_EXPORT bool CopyToClipboard(int uFormat, void* pData, size_t size, HWND hWnd = NULL);
	//�򿪼��а�
	extern UI_EXPORT bool GetClipboardData(int uFormat, std::function<void(void*, size_t)> Callback, HWND hWnd = NULL);
	//����unicode����
	extern UI_EXPORT bool CopyToClipboard(const std::wstring& str, HWND hWnd = NULL);
	//ճ��unicode����
	extern UI_EXPORT bool GetClipboardData(std::wstring* outStr, HWND hWnd = NULL);
	//��ѹ��Դ�ļ�
	extern UI_EXPORT bool UnZipResource(const EString& fileName, std::string* outData);
	//�ӻ�ȡ�ļ���Դ
	extern UI_EXPORT bool GetResource(const EString& fileName, std::string* outData);
	//��ȡ��ǰ���м���������Ϣ
	extern UI_EXPORT size_t GetMonitors(std::list<MonitorInfo>* outMonitorInfo);
	//��ȡ�û���ǰ���ڵ���ʾ��
	extern UI_EXPORT void GetMontior(MonitorInfo* outInfo, HWND hWnd = NULL);
	//���ع��
	extern UI_EXPORT HCURSOR LoadCursor(Cursor cursorType);
	//���ع��(//��Ҫ�ͷ�)
	extern UI_EXPORT HCURSOR LoadCursor(const EString& fileName);
	//�ͷŹ��
	extern UI_EXPORT void FreeCursor(HCURSOR hCursor);

	class UI_EXPORT Color :public RenderType::__Color {
	public:
		Color(const RenderType::__Color& copy) { this->RGBA = copy.GetValue(); }
		Color(const DWORD& rgba = 0) :RenderType::__Color(rgba) {}
		Color(BYTE r, BYTE g, BYTE b, BYTE a = 255) :RenderType::__Color(r, g, b, a) {}
	public:
		static Color Make(const EString& colorStr) {
			if (colorStr.find("#") == 0) { //"#4e6ef2"
				auto rStr = colorStr.substr(1, 2);
				auto gStr = colorStr.substr(3, 2);
				auto bStr = colorStr.substr(5, 2);
				unsigned int r, g, b;
				sscanf_s(rStr.c_str(), "%x", &r);
				sscanf_s(gStr.c_str(), "%x", &g);
				sscanf_s(bStr.c_str(), "%x", &b);
				//Argb = MakeARGB(255, r, g, b);
				return Color(r, g, b);
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
				return Color(r, g, b, a);
			}
			return Color();
		}
		virtual ~Color() {}
	};

#if USED_DIRECT2D
	class UI_EXPORT Image :public DXImage {
	public:
		virtual ~Image() {}
		Image(UINT width, UINT height) :DXImage(width, height) {}
		Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
		Image(IStream* iStream) :DXImage(iStream) {}
		Image(const std::wstring& fileName) :DXImage(fileName) {}
		Image(const void* data, size_t dataCount) :DXImage(data, dataCount) {}
	public:
		static Image* FromFile(const EString& fileOrRes) {
			//�����ļ��л�ȡ
			std::wstring wstr = fileOrRes.utf16();
			DWORD dwAttr = GetFileAttributesW(wstr.c_str());
			if (dwAttr && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
				return new Image(wstr);
			}
			//����Դ�л�ȡ
			std::string data;
			UnZipResource(fileOrRes, &data);
			if (data.empty()) {
				return NULL;
			}
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
	class UI_EXPORT StopWatch {
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
	class UI_EXPORT EventArgs {
	public:
		Event EventType = Event::None;
		EventArgs(const Event& eventType) {
			this->EventType = eventType;
		}
		virtual ~EventArgs() {};
	};
	//Ϊ����¼��ṩ��������
	class UI_EXPORT MouseEventArgs :public EventArgs {
	public:
		MouseButton Button = MouseButton::None;
		int ZDelta = 0;//����
		Point Location;
	public:
		MouseEventArgs(const Event& eventType, const Point& location = Point(0, 0), const MouseButton& mouseButton = MouseButton::None, const int& ZDelta = 0) :EventArgs(eventType) {
			this->Button = mouseButton;
			this->Location = location;
			this->ZDelta = ZDelta;
		}
		virtual ~MouseEventArgs() {}
	};
	// ժҪ: 
	//Ϊ�����¼��ṩ��������
	class UI_EXPORT KeyboardEventArgs :public EventArgs {
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
	class UI_EXPORT KillFocusEventArgs :public EventArgs {
	public:
		Control* Control;
		KillFocusEventArgs(EzUI::Control* ctl) :EventArgs(Event::OnKillFocus) {
			this->Control = ctl;
		}
		virtual ~KillFocusEventArgs() {}
	};
	//���귢���ı�
	class UI_EXPORT LocationEventArgs :public EventArgs {
	public:
		const EzUI::Point& Location;
		LocationEventArgs(const EzUI::Point& location) :EventArgs(Event::OnLocation), Location(location) {}
		virtual ~LocationEventArgs() {}
	};
	//��С�����ı�
	class UI_EXPORT SizeEventArgs :public EventArgs {
	public:
		const EzUI::Size& Size;
		SizeEventArgs(const EzUI::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//���η����ı�
	class UI_EXPORT RectEventArgs :public EventArgs {
	public:
		const EzUI::Rect& Rect;
		RectEventArgs(const EzUI::Rect& rect) :EventArgs(Event::OnRect), Rect(rect) {}
		virtual ~RectEventArgs() {}
	};
	class UI_EXPORT DpiChangeEventArgs :public EventArgs {
	public:
		float Scale = 1;
		DpiChangeEventArgs(const float& scale) :EventArgs(Event::OnDpiChange), Scale(scale) {}
		virtual ~DpiChangeEventArgs() {}
	};
	// Ϊ OnPaint �¼��ṩ���ݡ�
	class UI_EXPORT PaintEventArgs :public EventArgs {
	private:
		std::list<bool> layers;
		std::list<Point> offsets;
	public:
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::DXRender& Graphics;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		PaintEventArgs(EzUI::DXRender& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
		void PushLayer(const Rect& rectBounds);//�ٶȽϿ�
		void PushLayer(const Geometry& dxGeometry);//�ȽϺ�����,���ǿ������ο���ݲü�
		void PopLayer();//�������һ���ü�
		void PushOffset(const Point& offset);
		void PopOffset();//�������һ��ƫ��
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
		int FontSize = 0;//�����С ���м̳��� �˴�����int
		Color ForeColor;//ǰ����ɫ  ���м̳���
		HCURSOR Cursor = NULL;//�����ʽ
		float Angle = 0;//��ת��Χ 0~360
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
