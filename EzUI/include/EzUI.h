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
	//全局资源句柄
	extern UI_EXPORT HZIP HZipResource;//zip文件中的全局资源句柄
	extern UI_EXPORT HGLOBAL HVSResource;//vs中的资源文件句柄
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
	//解压文件
	//extern UI_EXPORT void UnZip(const EString& zipFileName, const EString& outPath, const EString& password = "", std::function<void(int index, int fileCount)> callback = NULL);
	//从获取文件资源
	extern UI_EXPORT bool GetResource(const EString& fileName, std::string* outData);
	//获取当前所有监视器的信息
	extern size_t GetMonitors(std::list<MonitorInfo>* outMonitorInfo);
	//获取用户当前正在操作的的显示器
	extern bool GetMontior(MonitorInfo* outInfo);
	extern HCURSOR LoadCursor(Cursor cursorType);
	extern HCURSOR LoadCursor(const EString& fileName);//需要释放
	extern void FreeCursor(HCURSOR hCursor);

	namespace Convert {
		inline Rect StringToRect(const EString& str) {
			auto rectStr = str.Split(",");
			Rect rect;
			if (str.empty()) {
				return rect;//如果没写矩形区域
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
		void Earse(const Rect& rect);//抹除矩形内容
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
			//从资源中获取
			std::string data;
			GetResource(fileOrRes, &data);
			return new Image(data.c_str(), data.size());
		}
	};
#endif
	// 定义用于保存显示器信息的结构体
	struct MonitorInfo {
		//显示器的位置 多显示器下Y轴可能出现负数或者大于0的时候代表显示器在设置里面显示器是错位的(多显示器没有平行);
		EzUI::Rect Rect;//逻辑宽高
		EzUI::Rect WorkRect;//工作区域
		Size Physical;//显示器物理宽高
		float Scale = 1.0f;//显示器缩放比例
		float FPS = 60;//显示器帧率
		bool Primary = false;//是否为主显示器
	};
	struct WindowData {
		void* Window = NULL;//窗口类实例
		float Scale = 1.0f;//缩放率
		Control* FocusControl = NULL;//具有焦点的控件
		Control* InputControl = NULL;//输入框
		size_t PaintCount = 0;
		bool Debug = false;//是否开启debug模式
		HWND HANDLE = NULL;//窗口句柄
		std::function<void(void*)> InvalidateRect = NULL;//使一个区域无效
		std::function<void()> UpdateWindow = NULL;//立即更新全部无效区域
		std::function<bool(Control*, EventArgs&)> Notify = NULL;//
		std::function<void(Control*)> RemoveControl = NULL;//清空控件标记等等...
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
		None = 1, //无状态 (无需布局)
		Pend = 2,//挂起中
		Layouting = 4//布局中
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
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};
	enum class ControlState {
		None,//无状态 则是使用_nowStyle缓存样式
		Static,//静态
		Checked,//选中状态
		Hover,//鼠标悬浮
		Active//鼠标按住
	};
	enum class DockStyle {
		// 摘要:
		//未设置
		None,
		// 摘要:
		//在父控件中 左右保持
		Horizontal,
		// 摘要:
		//在父控件中 上下保持
		Vertical,
		// 摘要:
		// 铺满整个父控件
		Fill
	};
	enum class MouseButton {
		// 摘要: 
		  //     未曾按下鼠标按钮。
		None,
		//
		// 摘要: 
		//     鼠标左按钮曾按下。
		Left,
		//
		// 摘要: 
		//     鼠标右按钮曾按下。
		Right,
		//
		// 摘要: 
		//     鼠标中按钮曾按下。
		Middle,
		//
		// 摘要: 
		//     第 1 个 XButton 曾按下。
		XButton1,
		//
		// 摘要: 
		//     第 2 个 XButton 曾按下。
		XButton2
	};
	enum class Cursor :ULONG_PTR
	{
		None = 0,//未指定
		APPSTARTING = (ULONG_PTR)IDC_APPSTARTING,// 标准的箭头和小沙漏
		ARROW = (ULONG_PTR)IDC_ARROW,// 标准的箭头
		CROSS = (ULONG_PTR)IDC_CROSS,// 十字光标
		HAND = (ULONG_PTR)IDC_HAND,// Windows 98/Me, Windows 2000/XP: Hand
		HELP = (ULONG_PTR)IDC_HELP,// 标准的箭头和问号
		IBEAM = (ULONG_PTR)IDC_IBEAM,// 工字光标
		ICON = (ULONG_PTR)IDC_ICON,// Obsolete for applications marked version 4.0 or later.
		NO = (ULONG_PTR)IDC_NO,// 禁止圈
		SIZE = (ULONG_PTR)IDC_SIZE,// Obsolete for applications marked version 4.0 or later. Use SIZEALL.
		SIZEALL = (ULONG_PTR)IDC_SIZEALL,// 四向箭头指向东、西、南、北
		SIZENESW = (ULONG_PTR)IDC_SIZENESW,// 双箭头指向东北和西南
		SIZENS = (ULONG_PTR)IDC_SIZENS, // 双箭头指向南北
		SIZENWSE = (ULONG_PTR)IDC_SIZENWSE,// 双箭头指向西北和东南
		SIZEWE = (ULONG_PTR)IDC_SIZEWE,// 双箭头指向东西
		UPARROW = (ULONG_PTR)IDC_UPARROW,// 垂直箭头
		WAIT = (ULONG_PTR)IDC_WAIT// 沙漏，Windows7下会显示为选择的圆圈表示等待
	};

	//基础事件
	class EventArgs {
	public:
		Event EventType = Event::None;
		EventArgs(const Event& eventType) {
			this->EventType = eventType;
		}
		virtual ~EventArgs() {};
	};
	//为鼠标事件提供基础数据
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
	//滚动条滚动事件
	class ScrollRollEventArgs :public EventArgs {
	public:
		int Pos = 0;
		int Total = 0;
		int Speed = 0;
		short ZDelta = 0;
		//滚动类型 一般分为三种 Event::OnMouseDrag Event::OnMouseWheel 其他
		Event RollType = Event::None;
	public:
		ScrollRollEventArgs(Event rollType = Event::None) :EventArgs(Event::OnScrollRoll), RollType(rollType) {
		}
		virtual ~ScrollRollEventArgs() {}
	};
	// 摘要: 
	//为键盘事件提供基础数据
	class KeyboardEventArgs :public EventArgs {
	public:
		/// <summary>
		/// 一般是指 键盘的ascii值
		/// </summary>
		WPARAM wParam;
		LPARAM lParam;
		KeyboardEventArgs(const Event& eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType) {
			this->wParam = wParam;
			this->lParam = lParam;
		}
		virtual ~KeyboardEventArgs() {}
	};
	//失去焦点
	class KillFocusEventArgs :public EventArgs {
	public:
		Control* Control;
		KillFocusEventArgs(EzUI::Control* ctl) :EventArgs(Event::OnKillFocus) {
			this->Control = ctl;
		}
		virtual ~KillFocusEventArgs() {}
	};
	//坐标发生改变
	class LocationEventArgs :public EventArgs {
	public:
		const EzUI::Point& Location;
		LocationEventArgs(const EzUI::Point& location) :EventArgs(Event::OnLocation), Location(location) {}
		virtual ~LocationEventArgs() {}
	};
	//大小发生改变
	class SizeEventArgs :public EventArgs {
	public:
		const EzUI::Size& Size;
		SizeEventArgs(const EzUI::Size& size) :EventArgs(Event::OnSize), Size(size) {}
		virtual ~SizeEventArgs() {}
	};
	//矩形发生改变
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
	// 为 OnPaint 事件提供数据。
	class PaintEventArgs :public EventArgs {
	public:
		std::list<Point> OffSetPoint;//用于记录每次绘制控件的偏移位置
		PaintEventArgs(const PaintEventArgs&) = delete;
		PaintEventArgs& operator=(const PaintEventArgs&) = delete;
		WindowData* PublicData = NULL;
		HDC DC = NULL;
		EzUI::DXRender& Graphics;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		PaintEventArgs(EzUI::DXRender& _painter) : EventArgs(Event::OnPaint), Graphics(_painter) {}
		virtual ~PaintEventArgs() {}
	};
	// 为控件样式提供数据。
	class UI_EXPORT ControlStyle {
	public:
		EzUI::Border Border;//边框信息
		//UI_Float Opacity;//整体不透明度
		Color BackColor = 0;//背景颜色
		Image* BackImage = NULL;//背景图片 如果指定的图片被删除 请必须将此置零
		Image* ForeImage = NULL;//前景图片 如果指定的图片被删除 请必须将此置零
		std::wstring FontFamily;//字体名称 具有继承性
		int FontSize = 0;//字体大小 具有继承性
		Color ForeColor;//前景颜色  具有继承性
		HCURSOR Cursor = NULL;//鼠标样式
	private:
		void operator=(const ControlStyle& right) {} //禁止直接赋值 因为这样会导致 Color执行拷贝使得Color变得不合法的有效
		ControlStyle(const ControlStyle& right) {} //禁止拷贝 
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
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//设置属性
		virtual EString GetAttribute(const EString& attrName);//获取属性
	};

};