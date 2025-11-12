#pragma once
#include "EzUI.h"

namespace ezui {
	namespace detail {
		//控件标志位
		enum class Flags :int16_t {
			Visible = 1,// 控件是否可见。此标志为 true 时,控件为显示状态
			Float = 2,// 控件是否浮动
			Pressed = 4,// 控件是否被为按住状态
			HitTestEnabled = 8,// 控件是否可以被命中(值为false情况下就是穿透效果)
			MouseTransparent = 16,// 控件自身不参与命中测试 (自身鼠标穿透,但是子控件依旧可以参与命中测试)
			AutoWidth = 32,// 是否根据内容自动宽度
			AutoHeight = 64,// 根据内容自动高度变化
			LayoutPending = 128,//等待布局
			InLayout = 256,//正在进行布局
		};
		EZUI_ENUM_OPERATORS(Flags, int16_t);
	};

	//所有控件基础类
	class UI_EXPORT Control :public Object
	{
		friend class HListView;
		friend class VListView;
		friend class TabLayout;
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
	private:
		class ControlContent :public Object {
		public:
			// 当前控件的 DPI 缩放比例
			float m_scale = 1.0f;
			// 父控件指针
			Control* m_parent = NULL;
			// 控件标志集合（可见、可点击等）
			detail::Flags m_flags = detail::Flags::Visible | detail::Flags::HitTestEnabled;
			// 当前控件视觉状态
			VisualState m_visualState = VisualState::Static;
			// 控件对应的窗口句柄（顶层控件有效）
			HWND m_hWnd = NULL;
			// 外边距
			Distance m_margin;
			// 内边距
			Distance m_padding;
			// 上次绘制时的位置
			Point m_lastLocation;
			// 上次绘制时的尺寸
			Size m_lastSize;
			// 内容区域尺寸
			Size m_contentSize;
			// 固定尺寸（优先级高于自适应）
			Size m_fixedSize;
			// 按比例缩放尺寸
			SizeF m_rateSize;
			// 最小尺寸
			Size m_minSize;
			// 最大尺寸
			Size m_maxSize;
			// 实际绘制矩形（相对父控件）
			Rect m_realRect;
			// 在客户端坐标系中的矩形
			Rect m_rectInClient;
			// 可视裁剪矩形
			Rect m_viewClipRect;
			// 控件是否仍存活（用于异步安全）
			std::shared_ptr<bool> m_alive;
			// 子控件集合
			ControlCollection m_controls;
			// 当前可见区域内的控件集合
			ControlCollection m_viewControls;
			// 浮动控件集合（悬浮层）
			ControlCollection m_floatCtrls;
			// 提示文本
			UIString m_tipsText;
			// 控件名称(唯一名称)
			UIString Name;
			// 默认样式
			ControlStyle Style;
			// 获取焦点时样式
			ControlStyle FocusStyle;
			// 禁用时样式
			ControlStyle DisabledStyle;
			// 悬停时样式
			ControlStyle HoverStyle;
			// 按下时样式
			ControlStyle ActiveStyle;
			// 内部样式表集合
			std::list<ezui::detail::Style> m_styles;
			// 图像资源管理器
			detail::PtrManager<Image*> m_imgs;
			// 控件事件处理回调
			std::function<void(Control*, EventArgs&)> EventHandler = NULL;
			ControlContent(Object* ownerObj) :Object(ownerObj) {}
			virtual ~ControlContent() {}
		};
	private:
		//保存控件中占用内存过大的变量 用于分担栈内存占用
		ControlContent* m_content = NULL;
	public:
		// 控件行为
		ControlAction Action = ControlAction::None;

		//是否添加到所在窗口/IFrame中的OnNotify函数中
		Event NotifyFlags = Event::OnMouseEvent | Event::OnKeyEvent;

		// 控件名称(唯一名称,同frame内不允许重复)
		UIString& Name;

		// 静态默认样式
		ControlStyle& Style;

		//具有焦点的时候的样式
		ControlStyle& FocusStyle;

		//禁用状态样式
		ControlStyle& DisabledStyle;

		// 鼠标悬浮样式
		ControlStyle& HoverStyle;

		// 鼠标按下样式
		ControlStyle& ActiveStyle;

		// 事件处理器
		std::function<void(Control*, EventArgs&)>& EventHandler;
	private:

		// 计算基于父控件的裁剪区域
		void ComputeClipRect();

		// 所有事件优先进入此函数(内部处理)
		void OnEvent(EventArgs& arg);

		//递归子控件给匹配成功的样式应用上
		void ApplyChildStyles(const std::list<ezui::detail::Style>& styles);

		//向上匹配样式(直到所属的Frame层)
		void ApplyParentStyles();

		//设置窗口句柄
		void SetWindowId(HWND hWnd);

		//计算带有策略的宽度
		int CalcWidth();

		//计算带有策略的度
		int CalcHeight();
	protected:
		//属性或者css样式都适用(css样式和属性都可以设置这些,只对静态样式生效)
		virtual bool ApplyStyleProperty(const UIString& key, const UIString& value);

		/// <summary>
		/// 为当前控件的指定状态设置单个样式属性
		/// </summary>
		/// <param name="style">目标状态样式对象,例如 this->HoverStyle</param>
		/// <param name="key">样式键名,例如 "font-size"</param>
		/// <param name="value">样式值,例如 "13px"</param>
		virtual void SetStyle(ControlStyle* style, const UIString& key, const UIString& value);

		// 获取当前控件状态下的样式信息(内部使用)
		virtual ControlStyle* GetStyle(VisualState _state);

		//获取当前控件的视觉状态
		VisualState GetVisualState();

		// 设置内容宽度，仅限子类使用
		virtual void SetContentWidth(int width);

		// 设置内容高度，仅限子类使用
		virtual void SetContentHeight(int height);

		// 设置内容尺寸，仅限子类使用
		virtual void SetContentSize(const Size& size);

		// 绘制之前
		virtual void OnPaintBefore(PaintEventArgs& args);

		// 控件绘制
		virtual void OnPaint(PaintEventArgs& args);

		// 子控件绘制，可重载此函数优化鼠标操作性能
		virtual void OnChildPaint(PaintEventArgs& args);

		// 背景绘制
		virtual void OnBackgroundPaint(PaintEventArgs& painter);

		// 前景绘制
		virtual void OnForePaint(PaintEventArgs& e);

		// 边框绘制
		virtual void OnBorderPaint(PaintEventArgs& painter, const Border& border);

		// 坐标发生改变
		virtual void OnMove(const MoveEventArgs& arg);

		// 大小发生改变
		virtual void OnSize(const SizeEventArgs& arg);

		// DPI 发生改变
		virtual void OnDpiChanged(const DpiChangedEventArgs& arg);

		// 控件布局逻辑，需重写布局请重写此函数
		virtual void OnLayout();

		//处理单个浮动控件
		virtual void OnLayoutFloatControl(Control* ctrl);

		// 鼠标在控件上移动
		virtual void OnMouseMove(const MouseEventArgs& arg);

		// 鼠标离开控件
		virtual void OnMouseLeave(const MouseEventArgs& args);

		// 鼠标滚轮事件
		virtual void OnMouseWheel(const MouseEventArgs& arg);

		// 鼠标按下事件
		virtual void OnMouseDown(const MouseEventArgs& arg);

		// 鼠标弹起事件
		virtual void OnMouseUp(const MouseEventArgs& arg);

		// 鼠标双击事件
		virtual void OnMouseDoubleClick(const MouseEventArgs& arg);

		// 鼠标移入控件
		virtual void OnMouseEnter(const MouseEventArgs& arg);

		// 鼠标事件统一入口
		virtual void OnMouseEvent(const MouseEventArgs& args);

		// 键盘事件统一入口
		virtual void OnKeyEvent(const KeyEventArgs& _args);

		// 字符输入事件（WM_CHAR）
		virtual void OnKeyChar(const KeyEventArgs& _args);

		// 键盘按下事件（WM_KEYDOWN）
		virtual void OnKeyDown(const KeyEventArgs& _args);

		// 键盘弹起事件（WM_KEYUP）
		virtual void OnKeyUp(const KeyEventArgs& _args);

		// 获得焦点事件
		virtual void OnFocus(const FocusEventArgs& _args);

		// 失去焦点事件
		virtual void OnKillFocus(const FocusEventArgs& _args);

		// 被移除时执行的逻辑
		virtual void OnRemove();

	public:
		// 获取当前控件状态的左上圆角半径
		Value<int16_t> GetBorderTopLeftRadius(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的右上圆角半径
		Value<int16_t> GetBorderTopRightRadius(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的右下圆角半径
		Value<int16_t> GetBorderBottomRightRadius(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的左下圆角半径
		Value<int16_t> GetBorderBottomLeftRadius(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的左边框宽度
		Value<int16_t> GetBorderLeft(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的上边框宽度
		Value<int16_t> GetBorderTop(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的右边框宽度
		Value<int16_t> GetBorderRight(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的下边框宽度
		Value<int16_t> GetBorderBottom(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的边框颜色
		Value<Color> GetBorderColor(VisualState _state = VisualState::Auto);

		//获取当前控件状态的边框样式
		Value<BorderStyle> GetBorderStyle(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的前景图片
		Value<Image*> GetForeImage(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的背景图片
		Value<Image*> GetBackImage(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的背景颜色
		Value<Color> GetBackColor(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的旋转角度
		Value<float> GetAngle(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的透明度
		Value<float> GetOpacity(VisualState _state = VisualState::Auto);

		//获取当前控件状态的的鼠标光标
		virtual Value<HCURSOR> GetCursor(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的前景颜色
		Value<Color> GetForeColor(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的字体 Family
		Value<std::wstring> GetFontFamily(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的字体大小
		Value<int> GetFontSize(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的字体粗度
		Value<int> GetFontWeight(VisualState _state = VisualState::Auto);

		// 获取当前控件状态的字体样式
		Value<FontStyle> GetFontStyle(VisualState _state = VisualState::Auto);
	public:

		// 构造函数 可传入父对象(由父对象自动管理内存)
		Control(Object* ownerObject = NULL);

		// 析构函数
		virtual ~Control();

		//绑定对象(跟随释放)
		using Object::Attach;

		//分离对象(解除跟随释放)
		using Object::Detach;

		//绑定图片(跟随释放)
		Image* Attach(Image* img);

		//分离图片(解除跟随释放)
		void Detach(Image* img);

		//分离并且立即释放
		void Free(Image* image);

		//分离并且立即释放
		void Free(Object* obj);

		//获取窗口句柄
		HWND GetWindowId();

		//获取主窗口实例
		Window* GetWindow();

		//获取主窗口中的公共数据
		const WindowContext* GetWindowContext();

		//获取当前控件所属的Frame容器
		Frame* GetFrame();

		// 以下函数请保证在父控件布局已完成的情况下使用，使用 ResumeLayout() 执行布局
		// 获取 X 坐标
		int X();

		// 获取 Y 坐标
		int Y();

		// 获取宽度
		int Width();

		// 获取高度
		int Height();

		// 设置 X 坐标
		void SetX(int X);

		// 设置 Y 坐标
		void SetY(int Y);

		// 移动相对于父控件的位置
		void SetLocation(const Point& pt, bool isScaled = true);

		// 设置控件大小（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetSize(const Size& size, bool isScaled = true);

		// 设置绝对宽高
		void SetFixedSize(const Size& size, bool isScaled = true);

		// 设置宽度（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetWidth(int width);

		// 设置高度（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetHeight(int height);

		// 设置绝对宽度
		void SetFixedWidth(int fixedWidth);

		// 设置绝对高度
		void SetFixedHeight(int fixedHeight);

		//设置基于父控件百分比宽度(0~1.0f)
		void SetRateWidth(float rateWidth);

		//设置基于父控件百分比高度(0~1.0f)
		void SetRateHeight(float rateHeight);

		// 设置基于父控件百分比宽高(0.0f~1.0f)
		void SetRateSize(const SizeF& size);

		/// <summary>
		/// 设置相对父控件矩形，返回实际的 rect
		/// </summary>
		/// <param name="rect"></param>
		/// <param name="isScaleSize">传入的矩形是否已经进行缩放过</param>
		/// <returns></returns>
		Rect SetRect(const Rect& rect, bool isScaled = true);

		// 获取绝对宽度
		int GetFixedWidth();

		// 获取绝对高度
		int GetFixedHeight();

		// 获取最小宽度
		int GetMinWidth();

		// 获取最小高度
		int GetMinHeight();

		// 获取最大宽度
		int GetMaxWidth();

		// 获取最大高度
		int GetMaxHeight();

		// 设置最小宽度
		void SetMinWidth(int w);

		// 设置最小高度
		void SetMinHeight(int h);

		// 设置最大宽度
		void SetMaxWidth(int w);

		// 设置最大高度
		void SetMaxHeight(int h);

		//设置最小size
		void SetMinSize(const Size& sz);

		//设置最大size
		void SetMaxSize(const Size& sz);

		// 获取光标位置
		virtual Rect GetCareRect();

		// 是否自动宽度
		virtual bool IsAutoWidth();

		// 是否自动高度
		virtual bool IsAutoHeight();

		// 设置自动宽度
		virtual void SetAutoWidth(bool flag);

		// 设置自动高度
		virtual void SetAutoHeight(bool flag);

		// 设置自动大小
		virtual void SetAutoSize(bool flag);

		// 获取控件内容大小
		virtual Size GetContentSize();

		// 获取控件大小
		Size GetSize();

		// 获取控件位置
		Point GetLocation();

		/// <summary>
		/// 获取相对于父控件的矩形
		/// </summary>
		/// <param name="bNativeSize">是否获取未经dpi缩放的大小</param>
		/// <returns>返回相对矩形位置</returns>
		virtual Rect GetRect(bool isScaled = true);

		// 获取基于客户端区域的矩形
		Rect GetRectInWindow();

		//获取控件基于屏幕的矩形位置
		Rect GetRectInScreen();

		//获取控件基于Frame层的矩形位置
		Rect GetRectInFrame();

		//控件是否已经进行dpi缩放处理
		bool IsScaled();

		// 获取控件的缩放系数
		float GetScale();

		//是否正在布局中
		bool IsInLayout();

		// 是否存在挂起的布局
		bool IsLayoutPending();

		// 标为为脏布局(将在合适的时机刷新布局)
		bool InvalidateLayout();

		// 结束布局状态
		void EndLayout();

		// 立即强制刷新布局
		virtual void RefreshLayout();

		// 设置提示文字（类似 tooltip）
		void SetTips(const UIString& text);

		// 获取提示文字
		const UIString& GetTips();

		// 获取控件的默认滚动条对象
		virtual ScrollBar* GetScrollBar();

		//获取垂直滚动条对象
		virtual VScrollBar* GetVScrollBar();

		//获取水平滚动条对象
		virtual HScrollBar* GetHScrollBar();

		// 派发事件（如鼠标单击事件等...）
		void SendEvent(const EventArgs& arg);

		// 设置控件属性
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue);

		// 获取当前可见的子控件集合
		const ControlCollection& GetCachedViewControls();

		//获取父控件
		Control* GetParent();

		//获取全部类名
		std::vector<UIString> GetClassList();

		//是否包含某个类名
		bool HasClass(const UIString& className);

		// 获取所有子控件集合
		const ControlCollection& GetControls();

		// 使用下标获取控件
		Control* GetControlAt(int pos);

		// 获取指定子控件的索引
		int IndexOf(Control* childCtl);

		// 根据 name 查找控件（包括自身）
		Control* FindControl(const UIString& ctrlName);

		// 根据属性查找所有匹配控件（包括自身）
		ControlCollection FindControls(const UIString& attrName, const UIString& attrValue);

		// 根据 name 查找子控件（仅限直接子集）
		Control* FindChild(const UIString& ctlName);

		// 根据属性查找所有匹配的子控件（仅限直接子集）
		ControlCollection FindChildren(const UIString& attrName, const UIString& attrValue);

		// 交换两个子控件的位置
		virtual bool SwapChildren(Control* childCtl, Control* childCt2);

		//是否启用控件
		void SetEnabled(bool flag);

		//是否禁用控件
		void SetDisabled(bool flag);

		//控件是否已启用
		bool IsEnabled();

		// 在指定位置插入子控件
		virtual Control* InsertAt(int pos, Control* childCtl);

		// 添加控件到末尾（如果是弹簧控件，在释放时将自动销毁）
		virtual Control* AddChild(Control* childCtrl);

		//解析xml字符串并添加到控件集合末尾
		virtual Control* Append(const UIString& xmlStr);

		//解析xml字符串并添加到控件集合第一位
		virtual Control* Prepend(const UIString& xmlStr);

		// 移除控件，freeCtrl 标志是否释放控件内存
		virtual void RemoveChild(Control* childCtl, bool freeCtrl = false);

		//移除并且销毁全部弹簧
		void DestroySpacers();

		// 设置控件的父控件
		virtual void SetParent(Control* parentCtl);

		//根据下标移除某个元素
		void RemoveAt(int pos);

		// 移除所有子控件
		virtual void RemoveAll();

		// 移除所有子控件，freeChilds 决定是否释放子控件内存
		virtual void RemoveAll(bool freeAll);

		//是否为弹簧控件
		virtual bool IsSpacer();

		//是否为Frame
		virtual bool IsFrame();

		// 设置控件浮动
		virtual void SetFloat(bool flag);

		// 控件是否浮动
		virtual bool IsFloat();

		// 设置控件可见性
		virtual void SetVisible(bool flag);

		// 获取控件可见性状态
		virtual bool IsVisible();

		//设置控件是否可以被鼠标命中(false则是鼠标穿透效果)
		void SetHitTestVisible(bool bEnable);

		//控件是否可以被命中
		bool IsHitTestVisible();

		//控件自身不参与命中测试 (自身鼠标穿透,但是子控件依旧可以参与命中测试)
		void SetMouseTransparent(bool bFlag);

		//控件自身是否可以被鼠标穿透
		bool IsMouseTransparent();

		//隐藏控件
		void Hide();

		//显示控件
		void Show();

		// 标记控件区域为无效（将会延迟刷新UI）
		virtual bool Invalidate();

		// 立即强制刷新控件区域并更新无效区域（且立即触发布局）
		virtual void Refresh();

		/// <summary>
		/// 为当前控件的指定状态批量设置样式（使用分号分隔）
		/// </summary>
		/// <param name="styleStr">样式字符串,例如 "font-size: 13px; color: #ffffff;"</param>
		/// <param name="state">控件状态,例如 ControlState::Hover</param>
		virtual void SetStyleSheet(const UIString& styleStr, VisualState state);

		/// <summary>
		/// 设置样式集合,并自动匹配应用到符合条件的子控件
		/// </summary>
		/// <param name="styleStr">样式字符串,例如 "#btn:hover { font-size:13px; }"</param>
		virtual void SetStyleSheet(const UIString& styleStr);

		//设置基于父控件的边距四周边距
		void SetMargin(int allMargin);

		//设置基于父控件的边距上下边距
		void SetMargin(int topBottom, int leftRight);

		//设置基于父控件的上边距
		void SetMarginTop(int topMargin);

		//设置基于父控件的左边距
		void SetMarginLeft(int leftMargin);

		//设置基于父控件的右边距
		void SetMarginRight(int rightMargin);

		//设置基于父控件的下边距
		void SetMarginBottom(int bottomMargin);

		//设置基于父控件的边距 上 右 下 左 
		void SetMargin(int top, int right, int bottom, int left);

		//获取基于父控件的边距信息；
		Distance GetMargin();

		// 设置控件四周的统一内边距
		void SetPadding(int allPadding);

		// 设置控件的上下、左右内边距
		// topBottom - 上下边距
		// leftRight - 左右边距
		void SetPadding(int topBottom, int leftRight);

		// 设置控件的上边距
		void SetPaddingTop(int topPadding);

		// 设置控件的左边距
		void SetPaddingLeft(int leftPadding);

		// 设置控件的右边距
		void SetPaddingRight(int rightPadding);

		// 设置控件的下边距
		void SetPaddingBottom(int bottomPadding);

		// 设置控件的四个方向内边距
		// top    - 上边距
		// right  - 右边距
		// bottom - 下边距
		// left   - 左边距
		void SetPadding(int top, int right, int bottom, int left);

		// 获取控件的内边距信息
		Distance GetPadding();

		//控件是否被按住
		bool IsPressed();

		//给当前控件设置为焦点控件
		void SetFocus();
	};

};