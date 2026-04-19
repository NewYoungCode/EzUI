#pragma once
#include "core/UIEvent.h"
#include "core/UICore.h"
#include "core/UIStyle.h"

namespace ezui {
	//控件集合
	using ControlCollection = Collection<Control*>;

	//描述控件行为
	enum class ControlAction :int16_t {
		None,
		Title,//具有移动窗口 双击最大化窗口的行为
		MoveWindow,//移动窗口
		Mini,//最小化
		Max,//最大化|恢复
		Close//关闭
	};

	namespace detail {
		/**
		 * @brief 控件标志位（位掩码）
		 *
		 * 说明：
		 * - 这些标志可按位组合，使用 EZUI_ENUM_OPERATORS 支持位运算。
		 * - 某些标志在父子关系下有优先或传递效果（例如 Visible 受父控件影响），详见成员说明。
		 */
		enum class Flags : int16_t {
			Visible = 1 << 0,				///< 控件是否可见（父控件隐藏时子控件也不可见）
			Float = 1 << 1,					///< 是否为浮动控件（不参与父布局，通常用于绝对定位/弹层）
			HitTestEnabled = 1 << 2,		///< 是否参与命中测试（可接收鼠标事件）
			MouseTransparent = 1 << 3,		///< 自身不参与命中测试，但子控件仍可接收事件（事件穿透自身）
			AutoWidth = 1 << 4,				///< 根据内容自动计算宽度（通常依赖 contentSize）
			AutoHeight = 1 << 5,			///< 根据内容自动计算高度
			LayoutPending = 1 << 6,			///< 已标记需要重新布局（脏状态，等待 RefreshLayout 处理）
			Focused = 1 << 7,				///< 当前是否拥有输入焦点（键盘输入目标）
			Checked = 1 << 8				///< 是否处于选中状态（如 CheckBox / RadioButton）
		};
		EZUI_ENUM_OPERATORS(Flags, int16_t);

		// 事件处理项：记录事件类型与对应的回调函数
		struct EventHandlerEntry;
	};

	///CtrlWeakPtr：专门用于窗口控件（Control）的弱指针类型，简化使用。
	template<typename T = Control>
	class CtrlWeakPtr : public WeakPtr<T> {
	public:
		CtrlWeakPtr(T* obj = nullptr) : WeakPtr<T>(obj, obj ? obj->GetAlive() : nullptr) {
		}
	};

	/// <summary>
	/// 所有控件的基类。
	/// 注：仅对公共 API 的行为进行文档说明，具体实现见对应 .cpp 文件。
	/// </summary>
	class EZUI_API Control : public Object
	{
		friend class HListView;
		friend class VListView;
		friend class DataGridView;
		friend class TabControl;
		friend class TileListView;
		friend class TreeView;
		friend class TextBox;
		friend class UILoader;
		friend class Frame;
		friend class CheckBox;
		friend class Window;
		friend class ScrollBar;
		friend class VLayout;
		friend class HLayout;
		friend class UIStyle;
		friend class ToolTip;
		friend class VScrollBar;
		friend class HScrollBar;
	private:
		struct ControlContext {
			// 控件是否存活的标志(单线程)
			SharedPtr<bool> m_ctrlAlive;
			/// 当前控件的 DPI 缩放比例。
			float m_scale = 1.0f;
			/// 控件标志集合（可见、是否参与命中测试等）。
			detail::Flags m_flags = detail::Flags::Visible | detail::Flags::HitTestEnabled | detail::Flags::LayoutPending;
			//是否按下鼠标按钮
			MouseButton m_pressedButton = MouseButton::None;
			/// 当前控件的伪样式状态（例如 Normal/Hover/Active/...）。
			VisualState m_visualState = VisualState::Normal;
			/// 父控件指针（若无父控件为 NULL）。
			Control* m_parent = NULL;
			/// 控件对应的窗口句柄（仅顶层控件有效）。
			WindowHandle m_hWnd = NULL;
			/// 外边距（相对父控件）。
			Distance m_margin;
			/// 内边距（控件内部内容偏移）。
			Distance m_padding;
			/// 内容区域尺寸（用于自动布局计算）。
			Size m_contentSize;
			/// 固定尺寸（优先于自适应/比率尺寸）。
			Size m_fixedSize;
			/// 按父控件比例设置的尺寸（0.0f~1.0f）。
			SizeF m_rateSize;
			/// 最小尺寸约束。
			Size m_minSize;
			/// 最大尺寸约束。
			Size m_maxSize;
			/// 实际绘制矩形（相对父控件坐标）。
			Rect m_realRect;
			/// 在客户端坐标系中的矩形（用于窗口坐标转换）。
			mutable Rect m_rectInClient;
			/// 可视裁剪矩形（用于裁剪子控件绘制）。
			Rect m_viewClipRect;
			/// 子控件集合（按添加顺序）。
			ControlCollection m_children;
			/// 图像资源管理（由控件统一管理附加的 Image 指针）。
			PtrManager<Image*> m_imgs;
			/// 内部样式表集合（用于选择器匹配）。
			std::list<StyleRule>* m_styles = NULL;
			//绑定的事件集合
			std::list<detail::EventHandlerEntry>* m_eventHandlers = NULL;
			/// 控件名称（可用于查找控件）。
			UIString* m_name = NULL;
			/// 元素类名集合（可用于样式选择器匹配）。
			std::vector<UIString>* m_classNames = NULL;
		};

		class EZUI_API RenderCache {
		public:
			/// 当前可见区域内的子控件集合（供性能优化缓存使用）。
			ControlCollection* m_drawnCtrls = NULL;

			// 将一个已绘制控件加入内部缓存
			void RecordDrawn(Control* ctrl)noexcept;

			// 从内部缓存移除一个已绘制控件
			void UnrecordDrawn(Control* ctrl)noexcept;

			// 清空所有已绘制控件缓存
			void ClearDrawnCache()noexcept;
		};

		class EZUI_API FloatCache {
		public:
			/// 浮动控件集合（悬浮层，不参与常规模块布局）。
			ControlCollection* m_floatCtrls = NULL;

			// 将浮动控件加入内部缓存
			void RecordFloat(Control* floatCtrl)noexcept;

			// 从内部缓存移除一个浮动控件
			void UnrecordFloat(Control* ctrl)noexcept;

			// 清空浮动控件缓存
			void ClearFloatCache()noexcept;

			// 获取当前缓存的所有浮动控件
			const ControlCollection& GetFloatCache() const noexcept;
		};
	private:
		// 控件运行上下文（包含与平台/环境相关的数据，如 DPI、窗口信息等）
		ControlContext m_ctx;

		// 渲染缓存管理（记录已绘制控件，用于优化渲染流程）
		RenderCache m_renderCache;

		// 浮动层控件缓存（管理浮动/Overlay 类型控件）
		FloatCache m_floatCache;
	public:
		/// 控件行为标识。
		ControlAction Action = ControlAction::None;

		/// 是否将控件添加到所在窗口/Frame 的通知（OnNotify）列表中。
		Event NotifyFlags = Event::MouseEvent | Event::KeyEvent;

		/// 静态（默认）样式引用。
		StyleProxy Style;

		/// 获取焦点时样式引用。
		StyleProxy FocusStyle;

		/// 禁用状态样式引用。
		StyleProxy DisabledStyle;

		/// 鼠标悬浮样式引用。
		StyleProxy HoverStyle;

		/// 鼠标按下样式引用。
		StyleProxy ActiveStyle;
	private:
		//重置状态
		void ResetState()noexcept;

		/// 计算基于父控件的裁剪区域（更新 m_viewClipRect 等）。
		void ComputeClipRect();

		/// 计算考虑策略（固定/比率/最小/最大/自动）的宽度值。
		/// 如果返回true则代表有size策略请使用outWidth的值
		bool TryCalcWidth(int& outWidth);

		/// 计算考虑策略（固定/比率/最小/最大/自动）的高度值。
		/// 如果返回true代表有size策略则请使用outHeight的值
		bool TryCalcHeight(int& outHeight);

		/// <summary>
		/// 设置相对父控件的矩形并返回实际应用的 rect。
		/// </summary>
		/// <param name="rect">相对于父控件的矩形。</param>
		/// <returns>返回应用后的实际矩形（相对父控件）。</returns>
		Rect SetRealRect(const Rect& dpiScaledRect);

		/// <summary>
		/// 设置相对父控件的矩形并返回实际应用的 rect。
		/// </summary>
		/// <param name="rect">相对于父控件的矩形。</param>
		/// <returns>返回应用后的实际矩形（相对父控件）。</returns>
		Rect SetRealRect(int x, int y, int width, int height);

		/// 所有事件首先进入此函数进行统一处理（内部使用）。
		void OnEvent(EventArgs* args);

		/// 递归对子树应用匹配成功的样式集合；遇到后代 Frame 仅作用于其自身，不进入其内部作用域（内部使用）。
		void ApplyChildStyles(const std::list<StyleRule>& styles);

		/// 将祖先作用域样式补应用到当前控件及其子树；遇到 Frame 仅作用于其自身，不进入其内部作用域（内部使用）。
		void ApplyParentStyles();

		//内部使用
		void ApplyBackImageStyle(StyleProxy& style);

		/// 设置窗口句柄（顶层控件专用，内部使用）。
		void SetHostWindowHandle(WindowHandle hwnd)noexcept;

		/// 根据 name 查找控件（包括自身）。
		Control* FindControlByName(const UIString& name);

		/// 根据 name 查找子控件（不包括自身）。
		Control* FindChildByName(const UIString& name);

		//调用事件处理器
		void InvokeEventHandlers(EventArgs* args);

		/// 准备绘制前(处理裁剪旋转 内部使用)
		void OnPaintBefore(PaintEventArgs* args);
	protected:
		/// 属性或 CSS 样式通用处理（仅对静态样式生效）。
		virtual bool ApplyStyleProperty(const UIString& key, const UIString& value);

		/// <summary>
		/// 为当前控件的指定状态设置单个样式属性（例如设置 this->HoverStyle 的某个属性）。
		/// </summary>
		/// <param name="style">目标样式对象，例如 this->HoverStyle。</param>
		/// <param name="key">样式键名，例如 "font-size"。</param>
		/// <param name="value">样式值，例如 "13px"。</param>
		virtual bool SetStyle(StyleProxy& style, const UIString& key, const UIString& value);

		/// 获取指定视觉状态下的样式（内部使用）。
		virtual StyleProxy& GetStyle(VisualState state);

		/// 获取当前控件的视觉状态。
		VisualState GetVisualState()noexcept;

		/// 设置内容宽度，仅供子类使用。
		virtual void SetContentWidth(int width);

		/// 设置内容高度，仅供子类使用。
		virtual void SetContentHeight(int height);

		/// 设置内容尺寸，仅供子类使用。
		virtual void SetContentSize(const Size& size);

		/// 控件绘制主函数（可重写）。
		virtual void OnPaint(PaintEventArgs* args);

		/// 子控件绘制（可在子类重载以优化鼠标交互性能）。
		virtual void OnChildPaint(PaintEventArgs* args);

		//绘制所有浮动控件的函数
		virtual void OnFloatsPaint(PaintEventArgs* args);

		/// 背景绘制（可重写）。
		virtual void OnBackgroundPaint(PaintEventArgs* args);

		/// 前景绘制（可重写）。
		virtual void OnForePaint(PaintEventArgs* args);

		/// 边框绘制（可重写）。
		virtual void OnBorderPaint(PaintEventArgs* args, const Border& border);

		/// 坐标变更回调。
		virtual void OnMove(MoveEventArgs* args);

		/// 大小变更回调。
		virtual void OnSize(SizeEventArgs* args);

		/// DPI 改变回调。
		virtual void OnDpiChanged(DpiChangedEventArgs* args);

		/// 控件布局逻辑，需自定义布局请重写此函数。布局完成请务必调用EndLayout函数
		virtual void OnLayout();

		/// 处理单个浮动控件布局（供布局实现使用）。
		virtual void OnLayoutFloatControl(Control* control);

		/// 鼠标移动事件回调。
		virtual void OnMouseMove(MouseEventArgs* args);

		/// 鼠标离开事件回调。
		virtual void OnMouseLeave(MouseEventArgs* args);

		/// 鼠标滚轮事件回调。
		virtual void OnMouseWheel(MouseEventArgs* args);

		/// 鼠标按下事件回调。
		virtual void OnMouseDown(MouseEventArgs* args);

		/// 鼠标弹起事件回调。
		virtual void OnMouseUp(MouseEventArgs* args);

		/// 鼠标单击事件
		virtual void OnMouseClick(MouseEventArgs* args);

		/// 鼠标双击事件回调。
		virtual void OnMouseDoubleClick(MouseEventArgs* args);

		/// 鼠标移入事件回调。
		virtual void OnMouseEnter(MouseEventArgs* args);

		/// 鼠标事件统一入口（内部使用）。
		virtual void OnMouseEvent(MouseEventArgs* args);

		/// 键盘事件统一入口（内部使用）。
		virtual void OnKeyEvent(KeyEventArgs* args);

		/// 字符输入事件（WM_CHAR）。
		virtual void OnKeyChar(KeyEventArgs* args);

		/// 键盘按下事件（WM_KEYDOWN）。
		virtual void OnKeyDown(KeyEventArgs* args);

		/// 键盘弹起事件（WM_KEYUP）。
		virtual void OnKeyUp(KeyEventArgs* args);

		/// 获得焦点事件回调。
		virtual void OnFocus(FocusEventArgs* args);

		/// 失去焦点事件回调。
		virtual void OnKillFocus(FocusEventArgs* args);

		/// 被移除时执行的逻辑（清理资源等）。
		virtual void OnRemove();

		// 当滚动偏移发生变化时触发（offset 为当前滚动偏移量）
		virtual void OnScroll(int offsetX, int offsetY);

		// 获取鼠标光标(根据鼠标位置获取光标,框架会优先调用此函数获取光标样式)
		virtual PropertyValue<HCURSOR> OnQueryCursor(const Point& point);
	public:
		/// 获取指定状态下左上角圆角半径。
		PropertyValue<int16_t> GetBorderTopLeftRadius(VisualState state = VisualState::None);

		/// 获取指定状态下右上角圆角半径。
		PropertyValue<int16_t> GetBorderTopRightRadius(VisualState state = VisualState::None);

		/// 获取指定状态下右下角圆角半径。
		PropertyValue<int16_t> GetBorderBottomRightRadius(VisualState state = VisualState::None);

		/// 获取指定状态下左下角圆角半径。
		PropertyValue<int16_t> GetBorderBottomLeftRadius(VisualState state = VisualState::None);

		/// 获取指定状态下左边框宽度。
		PropertyValue<int16_t> GetBorderLeft(VisualState state = VisualState::None);

		/// 获取指定状态上边框宽度。
		PropertyValue<int16_t> GetBorderTop(VisualState state = VisualState::None);

		/// 获取指定状态右边框宽度。
		PropertyValue<int16_t> GetBorderRight(VisualState state = VisualState::None);

		/// 获取指定状态下边框宽度。
		PropertyValue<int16_t> GetBorderBottom(VisualState state = VisualState::None);

		/// 获取指定状态的边框颜色。
		PropertyValue<Color> GetBorderColor(VisualState state = VisualState::None);

		/// 获取指定状态的边框样式。
		PropertyValue<BorderStyle> GetBorderStyle(VisualState state = VisualState::None);

		/// 获取指定状态的背景图片。
		PropertyValue<Image*> GetBackImage(VisualState state = VisualState::None);

		/// 获取指定状态的背景颜色。
		PropertyValue<Color> GetBackColor(VisualState state = VisualState::None);

		/// 获取指定状态的旋转角度（度数或弧度以实现规定为准）。
		PropertyValue<float> GetAngle(VisualState state = VisualState::None);

		/// 获取指定状态的透明度（0.0f~1.0f）。
		PropertyValue<float> GetOpacity(VisualState state = VisualState::None);

		/// 获取指定状态的鼠标光标。
		PropertyValue<HCURSOR> GetCursor(VisualState state = VisualState::None);

		/// 获取指定状态的前景颜色。
		PropertyValue<Color> GetForeColor(VisualState state = VisualState::None);

		/// 获取指定状态的字体族名称。
		PropertyValue<std::wstring> GetFontFamily(VisualState state = VisualState::None);

		/// 获取指定状态的字体大小
		PropertyValue<int> GetFontSize(VisualState state = VisualState::None);

		/// 获取指定状态的字体粗细（weight）。
		PropertyValue<int> GetFontWeight(VisualState state = VisualState::None);

		/// 获取指定状态的字体样式（例如正常/斜体）。
		PropertyValue<FontStyle> GetFontStyle(VisualState state = VisualState::None);
	public:
		/// 构造函数，可传入父对象（由父对象自动管理内存）。
		explicit Control(Object* owner = NULL);

		/// 析构函数。
		virtual ~Control();

		/// 获取控件名称（同Frame下唯一标识符）。
		const UIString& GetName() const;

		/// 设置控件名称（同Frame下唯一标识符）。
		void SetName(const UIString& ctrlName);

		/// 绑定对象（跟随释放）。
		using Object::Attach;

		/// 分离对象（解除跟随释放）。
		using Object::Detach;

		/// 绑定图片（跟随释放）。
		Image* Attach(Image* img);

		/// 分离图片（解除跟随释放）。
		void Detach(Image* img);

		/// 分离并立即释放图片资源。
		void Free(Image* image);

		/// 分离并立即释放对象。
		void Free(Object* obj);

		/// 获取承载该控件的窗口句柄（控件自身没有句柄）。
		WindowHandle GetHostWindowHandle() const noexcept;

		/// 获取承载该控件的主窗口实例（若存在）。
		Window* GetHostWindow() const noexcept;

		// 窗口桥接对象指针（用于 Control 与 Window 之间的通信）
		// 通过该对象调用窗口相关能力（如重绘、刷新、设置焦点等）
		const WindowBridge* GetWindowBridge()const noexcept;

		/// 获取当前控件所属的 Frame 容器（若存在）。
		Frame* GetFrame()const;

		/// 以下函数请确保在父控件布局已完成时使用，必要时调用 ResumeLayout() 执行布局。
		/// 获取 X 坐标（相对于父控件）。
		int X()const noexcept;

		/// 获取 Y 坐标（相对于父控件）。
		int Y()const noexcept;

		/// 获取控件宽度。
		int Width()const noexcept;

		/// 获取控件高度。
		int Height()const noexcept;

		/// 移动控件位置（相对于父控件）。
		void SetPosition(const Point& position);

		/// 移动控件位置（相对于父控件）。
		void SetPosition(int x, int y);

		/// 设置控件大小（频繁调用会影响性能，会触发 SetRect）。
		void SetSize(const Size& size);

		/// 设置控件大小（频繁调用会影响性能，会触发 SetRect）。
		void SetSize(int width, int height);

		/// 设置固定绝对宽高（优先于比率/自动,如果设置为0则代表取消固定size策略）。
		void SetFixedSize(const Size& size);

		/// 设置固定绝对宽高（优先于比率/自动,如果设置为0则代表取消固定size策略）。
		void SetFixedSize(int width, int height);

		/// 设置固定绝对宽度(优先级最高,如果设置为0则代表取消固定size策略)。
		void SetFixedWidth(int fixedWidth);

		/// 设置固定绝对高度(优先级最高,如果设置为0则代表取消固定size策略)。
		void SetFixedHeight(int fixedHeight);

		/// 设置宽度为父控件宽度的比例（0.0f ~ 1.0f,如果设置为0则代表取消比例策略）。
		/// 计算方式：width = parentWidth * rateWidth
		/// 注意：比例基于父控件的完整宽度，而不是布局后的剩余宽度。
		void SetRateWidth(float rateWidth);

		/// 设置高度为父控件高度的比例（0.0f ~ 1.0f,如果设置为0则代表取消比例策略）
		/// 计算方式：height = parentHeight * rateHeight
		/// 注意：比例基于父控件的完整高度，而不是布局后的剩余高度。
		void SetRateHeight(float rateHeight);

		/// 设置控件宽高占父控件尺寸的比例（0.0f ~ 1.0f,如果设置为0则代表取消比例策略）
		/// 实际尺寸：
		/// width  = parentWidth  * size.Width
		/// height = parentHeight * size.Height
		/// 该比例基于父控件的整体尺寸计算，不参与剩余空间分配。
		void SetRateSize(const SizeF& size);

		/// <summary>
		/// 设置相对父控件的矩形并返回实际应用的 rect。
		/// </summary>
		/// <param name="rect">相对于父控件的矩形。</param>
		/// <returns>返回应用后的实际矩形（相对父控件）。</returns>
		Rect SetRect(const Rect& dpiScaledRect);

		/// <summary>
		/// 设置相对父控件的矩形并返回实际应用的 rect。
		/// </summary>
		/// <param name="rect">相对于父控件的矩形。</param>
		/// <returns>返回应用后的实际矩形（相对父控件）。</returns>
		Rect SetRect(int x, int y, int width, int height);

		/// 获取固定宽度（未考虑比率/自动）。
		int GetFixedWidth()const noexcept;

		/// 获取固定高度（未考虑比率/自动）。
		int GetFixedHeight()const noexcept;

		/// 获取最小宽度约束。
		int GetMinWidth()const noexcept;

		/// 获取最小高度约束。
		int GetMinHeight()const noexcept;

		/// 获取最大宽度约束。
		int GetMaxWidth()const noexcept;

		/// 获取最大高度约束。
		int GetMaxHeight()const noexcept;

		/// 设置最小宽度。
		void SetMinWidth(int minWidth)noexcept;

		/// 设置最小高度。
		void SetMinHeight(int minHeight)noexcept;

		/// 设置最大宽度。
		void SetMaxWidth(int maxWidth)noexcept;

		/// 设置最大高度。
		void SetMaxHeight(int maxHeight)noexcept;

		/// 设置最小尺寸。
		void SetMinSize(const Size& minSize)noexcept;

		/// 设置最大尺寸。
		void SetMaxSize(const Size& maxSize)noexcept;

		/// 获取光标位置矩形（用于输入框等）。
		virtual Rect GetCaretRect()const;

		/// 是否自动宽度（根据内容）。
		virtual bool IsAutoWidth()const noexcept;

		/// 是否自动高度（根据内容）。
		virtual bool IsAutoHeight()const noexcept;

		/// 设置自动宽度开关。
		virtual void SetAutoWidth(bool flag);

		/// 设置自动高度开关。
		virtual void SetAutoHeight(bool flag);

		/// 同时设置自动宽高。
		virtual void SetAutoSize(bool flag);

		/// 获取控件内容大小（内容区域，不含内外边距）。
		virtual Size GetContentSize()const noexcept;

		/// 获取控件当前尺寸。
		Size GetSize()const noexcept;

		/// 获取控件相对父控件的位置。
		Point GetPosition()const noexcept;

		/// <summary>
		/// 获取相对于父控件的矩形。
		/// </summary>
		/// <returns>返回相对矩形位置。</returns>
		virtual Rect GetRect()const noexcept;

		/// 获取基于窗口客户端区域的矩形（窗口坐标系）。
		Rect GetRectInWindow()const;

		/// 获取控件在屏幕坐标系中的矩形位置。
		Rect GetRectInScreen()const;

		/// 获取控件在 Frame 层的矩形位置。
		Rect GetRectInFrame()const;

		/// 获取控件在指定父控件坐标系中的矩形。
		/// @param parent 必须是当前控件的父控件（直接或间接）。
		Rect GetRectIn(const Control* ctrl) const;

		/// 获取控件当前的缩放系数（DPI 缩放）。
		float GetScale()const noexcept;

		/// 设置控件的缩放系数（DPI 缩放）。
		void SetScale(float scale);

		/// 是否存在挂起的布局（已标记但未执行）。
		bool IsLayoutPending()const noexcept;

		/// 标记为脏布局，等待合适时机刷新布局。
		void InvalidateLayout() noexcept;

		/// 结束布局状态（布局更改之后必须调用此函数来结束布局状态）。
		void EndLayout()noexcept;

		/// 立即强制刷新布局（不推荐使用,会立即执行布局计算）。
		virtual void RefreshLayout();

		/// 设置提示文字tooltip(不传入则使用全局提示效果)
		void SetToolTip(const UIString& text, ToolTip* toolTip = NULL);

		/// 获取默认滚动条对象（若控件自带滚动条）。
		virtual ScrollBar* GetScrollBar();

		/// 获取垂直滚动条对象（若存在）。
		virtual VScrollBar* GetVScrollBar();

		/// 获取水平滚动条对象（若存在）。
		virtual HScrollBar* GetHScrollBar();

		/// 派发事件（如鼠标单击等）。
		void SendEvent(EventArgs* args);

		/// 派发事件（如鼠标单击等）。
		void SendEvent(const EventArgs& args);

		// Supported attributes (in addition to CSS-like properties):
		// name | id: string
		// style: inline css styles (e.g. "color: red; font-size: 14px")
		// float: true | false
		// location: x,y
		// size: width,height | auto
		// rect: x,y,width,height
		// visible: true | false
		// tooltip:  tooltip text
		// action: close | mini | max | title | move | movewindow
		// enable: true | false
		// disabled: (flag attribute, sets enable=false)
		// event: none (disables hit testing)
		// mousetransparent: true | false
		// 
		// CSS-like properties (handled by ApplyStyleProperty):
		// x, y, width, height, margin, padding, display, etc.
		// 
		// All other CSS properties are handled by SetStyle()
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue);

		/// 获取当前可见的子控件集合（缓存，用于渲染/命中测试优化）。
		const ControlCollection& GetDrawnCache()const;

		/// 获取父控件指针（若存在）。
		Control* GetParent() const noexcept;

		/// 获取控件类名列表（用于样式选择器）。
		const std::vector<UIString>& GetClassNames()const noexcept;

		/// 判断是否包含指定类名（用于样式匹配）。
		bool HasClass(const UIString& className)const noexcept;

		/// 获取所有子控件集合。
		const ControlCollection& GetChildren()const noexcept;

		/// 使用下标获取子控件。ignoreSpacer:是否忽略占位控件
		Control* GetChildAt(int pos, bool ignoreSpacer = false)const;

		/// 获取指定子控件的索引。ignoreSpacer:是否忽略占位控件
		int IndexOf(Control* child, bool ignoreSpacer = false)const;

		/// 在当前控件树使用 name 查找控件并转换类型（包括自身）。
		template<typename T = Control>
		T* FindControl(const UIString& ctrlName) {
			Control* ctrl = this->FindControlByName(ctrlName);
			if (std::is_same<T, Control>::value) {
				return (T*)ctrl;
			}
			T* out = dynamic_cast<T*>(ctrl);
#ifdef DEBUG
			if (ctrl && !out) {
				UIString err = "FindControl: failed to cast control '" + ctrlName + "' to target type.";
				EZUI_INFO(err.unicode().c_str());
				EZUI_ASSERT(!"Control Type Error!");
			}
#endif // DEBUG
			return out;
		}

		/// 在当前控件树使用中使用属性查找控件树匹配的控件（包括自身）。
		ControlCollection FindControls(const UIString& attrName, const UIString& attrValue);

		/// 根据 name 查找直接子控件（不递归）。
		template<typename T = Control>
		T* FindChild(const UIString& ctrlName) {
			Control* ctrl = this->FindChildByName(ctrlName);
			if (std::is_same<T, Control>::value) {
				return (T*)ctrl;
			}
			T* out = dynamic_cast<T*>(ctrl);
#ifdef DEBUG
			if (ctrl && !out) {
				UIString err = "FindChild: failed to cast control '" + ctrlName + "' to target type.";
				EZUI_INFO(err.unicode().c_str());
				EZUI_ASSERT(!"Control Type Error!");
			}
#endif // DEBUG
			return out;
		}

		/// 根据属性查找直接子控件（不递归）。
		ControlCollection FindChildren(const UIString& attrName, const UIString& attrValue);

		/// 交换两个子控件的位置（返回是否成功）。
		virtual bool SwapChild(Control* childCtrlA, Control* childCtrlB);

		/// 启用控件（取消禁用）。
		void SetEnabled(bool flag)noexcept;

		/// 控件是否已启用。
		bool IsEnabled()const noexcept;

		/// 在指定位置插入子控件。
		virtual Control* InsertChildAt(int pos, Control* childCtrl, bool autoDelete = false);

		/// 添加子控件到末尾（如果是弹簧控件，释放时将自动销毁）。
		virtual Control* AddChild(Control* childCtrl, bool autoDelete = false);

		/// 解析 XML 字符串并添加控件到集合末尾。 且该控件生命周期跟随父控件（父控件销毁时自动销毁该控件）。
		virtual Control* Append(const UIString& xml);

		/// 解析 XML 字符串并添加控件到集合开头。 且该控件生命周期跟随父控件（父控件销毁时自动销毁该控件）。
		virtual Control* Prepend(const UIString& xml);

		/// 移除子控件；freeChild 指定是否释放子控件内存。
		virtual bool RemoveChild(Control* childCtrl, bool freeChild = false);

		/// 移除并销毁全部弹簧控件。
		void DestroySpacers();

		/// 设置控件的父控件（改变父子关系）。
		virtual void SetParent(Control* parent);

		/// 根据下标移除某个子控件。freeChild 指定是否释放子控件内存。
		void RemoveChildAt(int pos, bool freeChild = false);

		/// 移除所有子控件，freeAll 决定是否释放子控件内存。
		virtual void RemoveAll(bool freeAll = false);

		/// 判断是否为弹簧控件（布局占位符）。
		virtual bool IsSpacer()const;

		/// 判断是否为 Frame。
		virtual bool IsFrame()const;

		/// 设置控件是否浮动（浮动控件不参与父布局）。
		virtual void SetFloat(bool flag);

		/// 控件是否浮动。
		virtual bool IsFloating()const noexcept;

		/// 设置控件可见性。
		virtual void SetVisible(bool flag);

		/// 获取控件当前可见性状态（考虑父控件影响）。
		virtual bool IsVisible()const noexcept;

		/// 设置控件是否参与命中测试（false 则鼠标穿透）。
		void SetHitTestVisible(bool bEnable);

		/// 控件是否参与命中测试。
		bool IsHitTestVisible()const noexcept;

		/// 设置控件自身是否对鼠标透明（自身不参与命中测试，但子控件可参与）。
		void SetMouseTransparent(bool bFlag)noexcept;

		/// 控件自身是否为鼠标穿透状态。
		bool IsMouseTransparent()const noexcept;

		/// 标记控件区域为无效，刷新 UI(推荐使用此函数来刷新界面)。
		virtual bool Invalidate();

		/// 立即刷新控件区域并更新无效区域（不推荐使用此函数来刷新UI,同时可能触发布局）。
		virtual void Refresh();

		/// <summary>
		/// 为当前控件的指定状态批量设置样式（使用分号分隔）。
		/// </summary>
		/// <param name="styleStr">样式字符串，例如 "font-size: 13px; color: #ffffff;"。</param>
		/// <param name="state">控件状态，例如 VisualState::Hover。</param>
		virtual void SetStyleSheet(const UIString& styleStr, VisualState state);

		/// <summary>
		/// 设置样式集合，并自动匹配应用到符合条件的子孙控件。
		/// </summary>
		/// <param name="styleStr">样式字符串，例如 "#btn:hover { font-size:13px; }"。</param>
		virtual void SetStyleSheet(const UIString& styleStr);

		/// 设置四周相同外边距。
		void SetMargin(int allMargin);

		/// 设置上下、左右外边距。
		void SetMargin(int topBottom, int leftRight);

		/// 设置上外边距。
		void SetMarginTop(int topMargin);

		/// 设置左外边距。
		void SetMarginLeft(int leftMargin);

		/// 设置右外边距。
		void SetMarginRight(int rightMargin);

		/// 设置下外边距。
		void SetMarginBottom(int bottomMargin);

		/// 设置外边距，顺序为：上、右、下、左。
		void SetMargin(int top, int right, int bottom, int left);

		/// 获取外边距信息（相对父控件）。
		Distance GetMargin()const noexcept;

		/// 设置四周相同内边距。
		void SetPadding(int allPadding);

		/// 设置上下、左右内边距。
		/// @param topBottom - 上下边距
		/// @param leftRight - 左右边距
		void SetPadding(int topBottom, int leftRight);

		/// 设置上内边距。
		void SetPaddingTop(int topPadding);

		/// 设置左内边距。
		void SetPaddingLeft(int leftPadding);

		/// 设置右内边距。
		void SetPaddingRight(int rightPadding);

		/// 设置下内边距。
		void SetPaddingBottom(int bottomPadding);

		/// 设置四方向内边距，顺序为：上、右、下、左。
		void SetPadding(int top, int right, int bottom, int left);

		/// 获取内边距信息。
		Distance GetPadding()const noexcept;

		/// 控件是否处于按下状态。(如果是 返回按钮 反之返回MouseButton::None)
		MouseButton PressedButton()const noexcept;

		/// 将当前控件设置焦点或取取消焦点
		void SetFocused(bool bLogicalFocus);

		//是否具有逻辑焦点(获取内部状态)
		bool IsFocused()const noexcept;

		// 添加事件处理器,返回句柄用于后续解绑
		// eventMask: 事件类型,支持位运算组合多个事件(如 Event::OnMouseDown | Event::OnMouseUp)
		// eventHandler: 事件回调函数
		// 返回值: 回调句柄,用于精确解绑该处理器
		EventHandle AddEventHandler(Event eventMask, const EventHandler& eventHandler);

		// 移除指定的事件处理器
		void RemoveEventHandler(EventHandle eventHandle);

	};

};
