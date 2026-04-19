#pragma once
#include "UICore.h"

namespace ezui {

	//事件回调函数
	using EventHandler = std::function<void(Control* sender, EventArgs* args)>;

	//事件回调句柄
	using EventHandle = void*;

	//事件类型(事件个数不得超过31个)
	enum class Event : uint32_t {
		None = 0,        // 无事件
		MouseWheel = 1,        // 鼠标滚轮滚动
		MouseEnter = 2,        // 鼠标进入控件区域
		MouseMove = 4,        // 鼠标移动
		MouseLeave = 8,        // 鼠标离开控件区域
		MouseDoubleClick = 16,       // 鼠标双击
		MouseDown = 32,       // 鼠标按键按下
		MouseUp = 64,       // 鼠标按键释放
		MouseClick = 128, // 鼠标单击
		KeyDown = 256,      // 键盘按键按下
		KeyUp = 512,      // 键盘按键释放
		Paint = 1024,      // 控件重绘
		Focus = 2048,     // 控件获得焦点
		KillFocus = 4096,     // 控件失去焦点
		KeyChar = 8192,     // 键盘字符输入
		Move = 16384,     // 控件位置改变
		Size = 32768,    // 控件大小改变
		Layout = 65536,    // 控件布局更新
		RectChanged = Move | Size,   // 位置或大小改变（Move | Size）
		DpiChanged = 131072,    // 屏幕 DPI 改变
		ControlAdded = 262144,   // 子控件被添加
		ControlRemoved = 524288,   // 子控件被移除
		MouseEvent = MouseWheel | MouseEnter | MouseMove | MouseLeave | MouseDoubleClick | MouseDown | MouseUp | MouseClick, // 所有鼠标事件
		KeyEvent = KeyDown | KeyUp | KeyChar,  // 所有键盘事件
	};
	EZUI_ENUM_OPERATORS(Event, uint32_t);


	//描述鼠标按下的键
	enum class MouseButton :int16_t {
		// 摘要: 
		  //     未曾按下鼠标按钮。
		None = 0,
		//
		// 摘要: 
		//     鼠标左按钮曾按下。
		Left = 1,
		//
		// 摘要: 
		//     鼠标右按钮曾按下。
		Right = 2,
		//
		// 摘要: 
		//     鼠标中按钮曾按下。
		Middle = 4,
		//
		// 摘要: 
		//     第 1 个 XButton 曾按下。
		XButton1 = 8,
		//
		// 摘要: 
		//     第 2 个 XButton 曾按下。
		XButton2 = 16
	};

	//基础事件参数
	class EZUI_API EventArgs {
		Event m_eventType;
	private:
		EventArgs(const EventArgs&) = delete;            // 禁止拷贝构造
		EventArgs& operator=(const EventArgs&) = delete; // 禁止拷贝赋值
		EventArgs(EventArgs&&) = delete;                 // 禁止移动构造
		EventArgs& operator=(EventArgs&&) = delete;      // 禁止移动赋值
	protected:
		EventArgs(Event eventType) :m_eventType(eventType) {}
		virtual ~EventArgs() {}
	public:
		//获取事件类型
		Event EventType()const noexcept {
			return m_eventType;
		}
		//尝试转为指定事件参数类型
		template<typename T>
		T* As() {
			T* out = dynamic_cast<T*>(this);
			return out;
		}
	};

	//为鼠标事件提供基础数据
	class EZUI_API MouseEventArgs final :public EventArgs {
		MouseButton m_button;
		int m_zDelta = 0;//方向
		Point m_position;
	public:
		//获取鼠标按键
		MouseButton Button()const noexcept {
			return m_button;
		}
		//获取鼠标滚轮滚动方向
		int ZDelta()const noexcept {
			return m_zDelta;
		}
		//获取鼠标坐标
		const Point& Position()const noexcept {
			return m_position;
		}
		//构造优先关注坐标
		MouseEventArgs(Event eventType, const Point& position = Point(0, 0), MouseButton mouseButton = MouseButton::None, int zDelta = 0) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_position(position)
			, m_zDelta(zDelta) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType);
		}
		//构造优先关注按钮
		MouseEventArgs(Event eventType, MouseButton mouseButton, const Point& position = Point(0, 0), int zDelta = 0) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_position(position)
			, m_zDelta(zDelta) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType);
		}
		//构造时优先zDelta
		MouseEventArgs(Event eventType, int zDelta, const Point& position = Point(0, 0), MouseButton mouseButton = MouseButton::None) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_position(position)
			, m_zDelta(zDelta) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType);
		}
		~MouseEventArgs() {}
	};
	// 摘要: 
	//为按键事件提供基础数据
	class EZUI_API KeyEventArgs final :public EventArgs {
		//按下的键值
		WPARAM m_keyCode;
		LPARAM m_lParam;
	public:
		//获取按键值
		WPARAM KeyCode()const noexcept {
			return m_keyCode;
		}
		//获取附加数据
		LPARAM LParam()const noexcept {
			return m_lParam;
		}
		KeyEventArgs(Event eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType), m_keyCode(wParam), m_lParam(lParam) {
			EZUI_ASSERT((eventType & Event::KeyEvent) == eventType);
		}
		~KeyEventArgs() {}
	};
	//为焦点相关事件提供基础数据
	class EZUI_API FocusEventArgs final :public EventArgs {
		//上一个具有焦点的控件
		Control* m_control;
	public:
		//获取上一个具有焦点的控件
		Control* Control()const noexcept {
			return m_control;
		}
		FocusEventArgs(Event eventType, ezui::Control* ctrl) :EventArgs(eventType), m_control(ctrl) {
			EZUI_ASSERT(eventType == Event::Focus || eventType == Event::KillFocus);//只能是其中一个 不然触发断言
		}
		~FocusEventArgs() {}
	};
	//坐标发生改变
	class EZUI_API MoveEventArgs final :public EventArgs {
		ezui::Point m_position;
	public:
		//获取坐标
		Point Position()const noexcept {
			return m_position;
		}
		MoveEventArgs(const ezui::Point& position) :EventArgs(Event::Move), m_position(position) {}
		~MoveEventArgs() {}
	};
	//大小发生改变
	class EZUI_API SizeEventArgs final :public EventArgs {
		ezui::Size m_size;
	public:
		//获取大小
		ezui::Size Size()const noexcept {
			return m_size;
		}
		SizeEventArgs(const ezui::Size& size) :EventArgs(Event::Size), m_size(size) {}
		~SizeEventArgs() {}
	};
	//dpi发生变化
	class EZUI_API DpiChangedEventArgs final :public EventArgs {
		float m_scale = 1.0f;
	public:
		//获取新的dpi缩放比例
		float Scale()const noexcept {
			return m_scale;
		}
		DpiChangedEventArgs(float scale) :EventArgs(Event::DpiChanged), m_scale(scale) {}
		~DpiChangedEventArgs() {}
	};
	//布局事件参数
	class EZUI_API LayoutEventArgs final :public EventArgs {
	public:
		LayoutEventArgs() :EventArgs(Event::Layout) {}
		~LayoutEventArgs() {}
	};
	// ControlAdded 事件参数
	class ControlAddedEventArgs final : public EventArgs {
		// 被添加的控件
		Control* m_control;
	public:
		// 获取被添加的控件
		Control* Control()const noexcept {
			return m_control;
		}
		ControlAddedEventArgs(ezui::Control* added) : EventArgs(Event::ControlAdded), m_control(added) {}
		~ControlAddedEventArgs() {}
	};
	// ControlRemoved 事件参数
	class ControlRemovedEventArgs final : public EventArgs {
		// 被移除的控件
		Control* m_control;
	public:
		// 获取被移除的控件
		Control* Control()const noexcept {
			return m_control;
		}
		ControlRemovedEventArgs(ezui::Control* removed) : EventArgs(Event::ControlRemoved), m_control(removed) {}
		~ControlRemovedEventArgs() {}
	};

	// 为 OnPaint 事件提供数据。
	class EZUI_API PaintEventArgs final :public EventArgs {
		std::list<bool> m_layers;
		std::list<ezui::Graphics::TransformState> m_offsets;
		ezui::Graphics* m_graphics;//画家
		const WindowBridge* m_windowBridge;//Control与Window桥接对象
		std::vector<Rect> m_invalidRectangle;//WM_PAINT里面的无效区域集合
		ezui::WindowHandle m_windowHandle;//主窗口句柄
	public:
		int PaintCount = 0;
		//获取画家
		ezui::Graphics* Graphics()const noexcept {
			return m_graphics;
		}
		// 窗口桥接对象指针（用于 Control 与 Window 之间的通信）
		// 通过该对象调用窗口相关能力（如重绘、刷新、设置焦点等）
		const WindowBridge* WindowBridge()const noexcept {
			return m_windowBridge;
		}
		//获取无效区域集合
		const std::vector<Rect>& InvalidRectangles()const noexcept {
			return m_invalidRectangle;
		}
		//获取主窗口句柄
		ezui::WindowHandle WindowHandle()const noexcept {
			return m_windowHandle;
		}
	public:
		PaintEventArgs(ezui::Graphics* painter, const ezui::WindowBridge* windowContext, ezui::WindowHandle hWnd, const std::vector<Rect>& invalidRectangle) : EventArgs(Event::Paint)
			, m_graphics(painter)
			, m_windowBridge(windowContext)
			, m_invalidRectangle(invalidRectangle)
			, m_windowHandle(hWnd)
		{
		}
		~PaintEventArgs() {}
		void PushLayer(const Geometry& dxGeometry, float opacity) {
			this->m_graphics->PushLayer(dxGeometry, opacity);
			m_layers.push_back(false);
		}
		void PushLayer(const Rect& rectBounds) {
			this->m_graphics->PushAxisAlignedClip(rectBounds);
			m_layers.push_back(true);
		}
		void PopLayer() {
			if (m_layers.size() > 0) {
				if (*m_layers.rbegin() == true) {
					this->m_graphics->PopAxisAlignedClip();
				}
				else {
					this->m_graphics->PopLayer();
				}
				m_layers.pop_back();
			}
		}
		void PushOffset(const Point& offset)
		{
			this->m_offsets.emplace_back(this->m_graphics->GetTransformState());
			this->m_graphics->SetTransform((float)offset.X, (float)offset.Y);
		}
		void PopOffset()
		{
			if (this->m_offsets.size() > 0) {
				auto state = this->m_offsets.back();
				this->m_offsets.pop_back();
				this->m_graphics->RestoreTransformState(state);
			}
			else
			{
				this->m_graphics->RestoreTransformState({});
			}
		}
	};
};
