#pragma once
#include "UICore.h"

namespace ezui {

	//事件回调函数
	typedef std::function<void(Control* sender, EventArgs* args)> EventHandler;

	//事件回调句柄
	typedef void* EventHandle;

	//事件类型(事件个数不得超过31个)
	EZUI_SCOPED_ENUM_BEGIN(Event, uint32_t) {
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
		DragEnter = 1048576,    // 文件拖拽进入控件
		DragOver = 2097152,     // 文件拖拽悬停在控件上
		DragMove = DragOver,    // DragOver 的兼容别名
		DragLeave = 4194304,    // 文件拖拽离开控件
		Drop = 8388608,         // 文件拖拽释放到控件
		MouseEvent = MouseWheel | MouseEnter | MouseMove | MouseLeave | MouseDoubleClick | MouseDown | MouseUp | MouseClick, // 所有鼠标事件
		KeyEvent = KeyDown | KeyUp | KeyChar,  // 所有键盘事件
		DragEvent = DragEnter | DragOver | DragLeave | Drop, // 所有文件拖拽事件
	} EZUI_SCOPED_ENUM_END(Event)
	EZUI_ENUM_OPERATORS(Event, uint32_t);


	//描述鼠标按下的键
	EZUI_SCOPED_ENUM_BEGIN(MouseButton, int16_t) {
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
	} EZUI_SCOPED_ENUM_END(MouseButton)

	//描述文件拖拽目标允许的操作
	EZUI_SCOPED_ENUM_BEGIN(DragDropEffect, uint32_t) {
		None = 0,
		Copy = 1,
		Move = 2,
		Link = 4
	} EZUI_SCOPED_ENUM_END(DragDropEffect)
	EZUI_ENUM_OPERATORS(DragDropEffect, uint32_t);

	//基础事件参数
	class EZUI_API EventArgs {
		Event m_eventType;
	private:
		EventArgs(const EventArgs&);            // 禁止拷贝构造
		EventArgs& operator=(const EventArgs&); // 禁止拷贝赋值
		EventArgs(EventArgs&&);                 // 禁止移动构造
		EventArgs& operator=(EventArgs&&);      // 禁止移动赋值
	protected:
		EventArgs(Event eventType) :m_eventType(eventType) {}
		virtual ~EventArgs() {}
	public:
		//获取事件类型
		Event EventType()const EZUI_NOEXCEPT {
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
	class EZUI_API MouseEventArgs EZUI_FINAL :public EventArgs {
		MouseButton m_button;
		int m_zDelta;//方向
		Point m_position;
	public:
		//获取鼠标按键
		MouseButton Button()const EZUI_NOEXCEPT {
			return m_button;
		}
		//获取鼠标滚轮滚动方向
		int ZDelta()const EZUI_NOEXCEPT {
			return m_zDelta;
		}
		//获取鼠标坐标
		const Point& Position()const EZUI_NOEXCEPT {
			return m_position;
		}
		//构造优先关注坐标
		MouseEventArgs(Event eventType, const Point& position = Point(0, 0), MouseButton mouseButton = MouseButton::None, int zDelta = 0) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_zDelta(zDelta)
			, m_position(position) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType, L"MouseEventArgs requires a mouse event type");
		}
		//构造优先关注按钮
		MouseEventArgs(Event eventType, MouseButton mouseButton, const Point& position = Point(0, 0), int zDelta = 0) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_zDelta(zDelta)
			, m_position(position) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType, L"MouseEventArgs requires a mouse event type");
		}
		//构造时优先zDelta
		MouseEventArgs(Event eventType, int zDelta, const Point& position = Point(0, 0), MouseButton mouseButton = MouseButton::None) :EventArgs(eventType)
			, m_button(mouseButton)
			, m_zDelta(zDelta)
			, m_position(position) {
			EZUI_ASSERT((eventType & Event::MouseEvent) == eventType, L"MouseEventArgs requires a mouse event type");
		}
		~MouseEventArgs() {}
	};
	// 摘要: 
	//为按键事件提供基础数据
	class EZUI_API KeyEventArgs EZUI_FINAL :public EventArgs {
		//按下的键值
		WPARAM m_keyCode;
		LPARAM m_lParam;
	public:
		//获取按键值
		WPARAM KeyCode()const EZUI_NOEXCEPT {
			return m_keyCode;
		}
		//获取附加数据
		LPARAM LParam()const EZUI_NOEXCEPT {
			return m_lParam;
		}
		KeyEventArgs(Event eventType, WPARAM wParam, LPARAM lParam) :EventArgs(eventType), m_keyCode(wParam), m_lParam(lParam) {
			EZUI_ASSERT((eventType & Event::KeyEvent) == eventType, L"KeyEventArgs requires a key event type");
		}
		~KeyEventArgs() {}
	};
	//为文件拖拽事件提供基础数据
	class EZUI_API FileDragEventArgs EZUI_FINAL :public EventArgs {
		std::vector<UIString> m_files;
		Point m_position;
		Point m_windowPosition;
		uint32_t m_keyState;
		DragDropEffect m_allowedEffect;
		DragDropEffect m_effect;
		bool m_accepted;
	private:
		DragDropEffect NormalizeEffect(DragDropEffect effect)const {
			if (effect == DragDropEffect::None) {
				return DragDropEffect::None;
			}
			DragDropEffect requestedEffect = effect & m_allowedEffect;
			if (static_cast<uint32_t>(DragDropEffect::Copy & requestedEffect) != 0) {
				return DragDropEffect::Copy;
			}
			if (static_cast<uint32_t>(DragDropEffect::Move & requestedEffect) != 0) {
				return DragDropEffect::Move;
			}
			if (static_cast<uint32_t>(DragDropEffect::Link & requestedEffect) != 0) {
				return DragDropEffect::Link;
			}
			if (static_cast<uint32_t>(DragDropEffect::Copy & m_allowedEffect) != 0) {
				return DragDropEffect::Copy;
			}
			if (static_cast<uint32_t>(DragDropEffect::Move & m_allowedEffect) != 0) {
				return DragDropEffect::Move;
			}
			if (static_cast<uint32_t>(DragDropEffect::Link & m_allowedEffect) != 0) {
				return DragDropEffect::Link;
			}
			return DragDropEffect::None;
		}
	public:
		//获取拖拽进入的文件列表
		const std::vector<UIString>& Files()const EZUI_NOEXCEPT {
			return m_files;
		}
		//获取相对当前控件的坐标
		const Point& Position()const EZUI_NOEXCEPT {
			return m_position;
		}
		//获取相对窗口客户区的坐标
		const Point& WindowPosition()const EZUI_NOEXCEPT {
			return m_windowPosition;
		}
		//获取原始按键状态，对应 Windows 的 grfKeyState
		uint32_t KeyState()const EZUI_NOEXCEPT {
			return m_keyState;
		}
		//获取拖拽源允许的操作
		DragDropEffect AllowedEffect()const EZUI_NOEXCEPT {
			return m_allowedEffect;
		}
		//获取当前控件接受后的操作
		DragDropEffect Effect()const EZUI_NOEXCEPT {
			return m_effect;
		}
		//当前控件是否接受此次拖拽
		bool IsAccepted()const EZUI_NOEXCEPT {
			return m_accepted;
		}
		//接受此次拖拽，默认按复制处理；若拖拽源不允许复制，会自动降级到允许的操作。
		void Accept(DragDropEffect effect = DragDropEffect::Copy)EZUI_NOEXCEPT {
			m_effect = NormalizeEffect(effect);
			m_accepted = static_cast<uint32_t>(m_effect) != 0;
		}
		//拒绝此次拖拽
		void Reject()EZUI_NOEXCEPT {
			m_effect = DragDropEffect::None;
			m_accepted = false;
		}
		FileDragEventArgs(Event eventType, const std::vector<UIString>& files, const Point& position,
			const Point& windowPosition, uint32_t keyState = 0,
			DragDropEffect allowedEffect = DragDropEffect::Copy)
			:EventArgs(eventType)
			, m_files(files)
			, m_position(position)
			, m_windowPosition(windowPosition)
			, m_keyState(keyState)
			, m_allowedEffect(allowedEffect)
			, m_effect(DragDropEffect::None)
			, m_accepted(false) {
			EZUI_ASSERT((eventType & Event::DragEvent) == eventType, L"FileDragEventArgs requires a drag event type");
		}
		~FileDragEventArgs() {}
	};
	//为焦点相关事件提供基础数据
	class EZUI_API FocusEventArgs EZUI_FINAL :public EventArgs {
		//上一个具有焦点的控件
		Control* m_control;
	public:
		//获取上一个具有焦点的控件
		Control* Control()const EZUI_NOEXCEPT {
			return m_control;
		}
		FocusEventArgs(Event eventType, ezui::Control* ctrl) :EventArgs(eventType), m_control(ctrl) {
			EZUI_ASSERT(eventType == Event::Focus || eventType == Event::KillFocus, L"FocusEventArgs requires Focus or KillFocus");//只能是其中一个 不然触发断言
		}
		~FocusEventArgs() {}
	};
	//坐标发生改变
	class EZUI_API MoveEventArgs EZUI_FINAL :public EventArgs {
		ezui::Point m_position;
	public:
		//获取坐标
		Point Position()const EZUI_NOEXCEPT {
			return m_position;
		}
		MoveEventArgs(const ezui::Point& position) :EventArgs(Event::Move), m_position(position) {}
		~MoveEventArgs() {}
	};
	//大小发生改变
	class EZUI_API SizeEventArgs EZUI_FINAL :public EventArgs {
		ezui::Size m_size;
	public:
		//获取大小
		ezui::Size Size()const EZUI_NOEXCEPT {
			return m_size;
		}
		SizeEventArgs(const ezui::Size& size) :EventArgs(Event::Size), m_size(size) {}
		~SizeEventArgs() {}
	};
	//dpi发生变化
	class EZUI_API DpiChangedEventArgs EZUI_FINAL :public EventArgs {
		float m_scale;
	public:
		//获取新的dpi缩放比例
		float Scale()const EZUI_NOEXCEPT {
			return m_scale;
		}
		DpiChangedEventArgs(float scale) :EventArgs(Event::DpiChanged), m_scale(scale) {}
		~DpiChangedEventArgs() {}
	};
	//布局事件参数
	class EZUI_API LayoutEventArgs EZUI_FINAL :public EventArgs {
	public:
		LayoutEventArgs() :EventArgs(Event::Layout) {}
		~LayoutEventArgs() {}
	};
	// ControlAdded 事件参数
	class ControlAddedEventArgs EZUI_FINAL : public EventArgs {
		// 被添加的控件
		Control* m_control;
	public:
		// 获取被添加的控件
		Control* Control()const EZUI_NOEXCEPT {
			return m_control;
		}
		ControlAddedEventArgs(ezui::Control* added) : EventArgs(Event::ControlAdded), m_control(added) {}
		~ControlAddedEventArgs() {}
	};
	// ControlRemoved 事件参数
	class ControlRemovedEventArgs EZUI_FINAL : public EventArgs {
		// 被移除的控件
		Control* m_control;
	public:
		// 获取被移除的控件
		Control* Control()const EZUI_NOEXCEPT {
			return m_control;
		}
		ControlRemovedEventArgs(ezui::Control* removed) : EventArgs(Event::ControlRemoved), m_control(removed) {}
		~ControlRemovedEventArgs() {}
	};

	// 为 OnPaint 事件提供数据。
	class EZUI_API PaintEventArgs EZUI_FINAL :public EventArgs {
		std::list<bool> m_layers;
		std::list<ezui::Graphics::TransformState> m_offsets;
		ezui::Graphics* m_graphics;//画家
		const WindowBridge* m_windowBridge;//Control与Window桥接对象
		std::vector<Rect> m_invalidRectangle;//WM_PAINT里面的无效区域集合
		ezui::WindowHandle m_windowHandle;//主窗口句柄
	public:
		int PaintCount;
		//获取画家
		ezui::Graphics* Graphics()const EZUI_NOEXCEPT {
			return m_graphics;
		}
		// 窗口桥接对象指针（用于 Control 与 Window 之间的通信）
		// 通过该对象调用窗口相关能力（如重绘、刷新、设置焦点等）
		const WindowBridge* WindowBridge()const EZUI_NOEXCEPT {
			return m_windowBridge;
		}
		//获取无效区域集合
		const std::vector<Rect>& InvalidRectangles()const EZUI_NOEXCEPT {
			return m_invalidRectangle;
		}
		//获取主窗口句柄
		ezui::WindowHandle WindowHandle()const EZUI_NOEXCEPT {
			return m_windowHandle;
		}
	public:
		PaintEventArgs(ezui::Graphics* painter, const ezui::WindowBridge* windowContext, ezui::WindowHandle hWnd, const std::vector<Rect>& invalidRectangle) : EventArgs(Event::Paint)
			, m_layers()
			, m_offsets()
			, m_graphics(painter)
			, m_windowBridge(windowContext)
			, m_invalidRectangle(invalidRectangle)
			, m_windowHandle(hWnd)
			, PaintCount(0)
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
			this->m_offsets.push_back(this->m_graphics->GetTransformState());
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
				this->m_graphics->RestoreTransformState(ezui::Graphics::TransformState());
			}
		}
	};
};
