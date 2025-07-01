#pragma once
#include "EzUI.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		// 控件是否已经被移除或释放
		bool* _isRemove = NULL;

		// 控件是否可见。此标志为 true 时,控件为显示状态
		bool _visible = true;

		// 当前控件的 DPI 缩放比例
		float _scale = 1.0f;

		// 子控件集合
		Controls _controls;

		// 存储弹簧控件的集合
		Controls _spacers;

		// 布局状态
		// AddControl、InsertControl、RemoveControl、OnSize 时此标志为挂起状态
		// 调用 ResumeLayout 标志为布局中
		// 调用 OnLayout() 之后标志为 None
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;

		// 鼠标悬浮提示文字
		EString _tipsText;

		// 上一次位置
		Point _lastLocation;

		// 上一次大小
		Size _lastSize;

		// 是否根据内容自动宽度
		bool _autoWidth = false;

		// 根据内容自动高度变化
		bool _autoHeight = false;

		// 控件内容宽高
		Size _contentSize;

		// 绝对尺寸
		Size _fixedSize;

		// 控件矩形区域（基于父控件）
		Rect _rect;

		// 控件在窗口中的可见区域
		Rect _viewRect;

		// dock 样式
		DockStyle _dock = DockStyle::None;

	private:
		// 禁止拷贝构造
		Control(const Control&) = delete;

		// 禁止赋值
		Control& operator=(const Control&) = delete;

		// 计算基于父控件的裁剪区域
		void ComputeClipRect();

	protected:
		// 基于控件中的可见控件集合
		Controls ViewControls;

	public:
		// 外边距
		// 让容器独占一行或一列时，设置边距会使控件变小，不可设置为负数
		Distance Margin;

		// 控件的 ObjectName ID
		EString Name;

		// 控件当前状态
		ControlState State = ControlState::Static;

		// 控件行为
		ControlAction Action = ControlAction::None;

		// 默认样式
		ControlStyle Style;

		// 鼠标悬浮样式
		ControlStyle HoverStyle;

		// 鼠标按下样式
		ControlStyle ActiveStyle;

		// 父控件指针
		Control* Parent = NULL;

		// 控件是否启用，禁止状态下鼠标键盘消息将不可用
		bool Enable = true;

		// 添加到主窗口通知函数中可拦截的事件
		Event EventFilter = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnKeyChar | Event::OnKeyDown | Event::OnKeyUp;

		// 控件可被穿透的事件
		Event EventPassThrough = Event::None;

		// 事件处理器
		std::function<void(Control*, EventArgs&)> EventHandler = NULL;

	protected:
		// 设置内容宽度，仅限子类使用
		virtual void SetContentWidth(int_t width);

		// 设置内容高度，仅限子类使用
		virtual void SetContentHeight(int_t height);

		// 设置内容尺寸，仅限子类使用
		virtual void SetContentSize(const Size& size);

		// 所有事件优先进入此函数
		virtual bool OnNotify(EventArgs& arg);

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
		virtual void OnDpiChange(const DpiChangeEventArgs& arg);

		// 控件布局逻辑，需重写布局请重写此函数
		virtual void OnLayout();

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

		// 鼠标单击事件
		virtual void OnMouseClick(const MouseEventArgs& arg);

		// 鼠标双击事件
		virtual void OnMouseDoubleClick(const MouseEventArgs& arg);

		// 鼠标移入控件
		virtual void OnMouseEnter(const MouseEventArgs& arg);

		// 鼠标事件统一入口
		virtual void OnMouseEvent(const MouseEventArgs& args);

		// 键盘事件统一入口
		virtual void OnKeyBoardEvent(const KeyboardEventArgs& _args);

		// 字符输入事件（WM_CHAR）
		virtual void OnKeyChar(const KeyboardEventArgs& _args);

		// 键盘按下事件（WM_KEYDOWN）
		virtual void OnKeyDown(const KeyboardEventArgs& _args);

		// 键盘弹起事件（WM_KEYUP）
		virtual void OnKeyUp(const KeyboardEventArgs& _args);

		// 获得焦点事件
		virtual void OnFocus(const FocusEventArgs& _args);

		// 失去焦点事件
		virtual void OnKillFocus(const KillFocusEventArgs& _args);

		// 被移除时执行的逻辑
		virtual void OnRemove();

	public:
		// 获取当前控件状态下的样式信息
		virtual ControlStyle& GetStyle(const ControlState& _state);

		// 获取默认样式（用于不同控件状态下的样式）
		virtual ControlStyle& GetDefaultStyle();

		// 获取左上圆角半径
		int_t GetBorderTopLeftRadius(ControlState _state = ControlState::None);

		// 获取右上圆角半径
		int_t GetBorderTopRightRadius(ControlState _state = ControlState::None);

		// 获取右下圆角半径
		int_t GetBorderBottomRightRadius(ControlState _state = ControlState::None);

		// 获取左下圆角半径
		int_t GetBorderBottomLeftRadius(ControlState _state = ControlState::None);

		// 获取左边框宽度
		int_t GetBorderLeft(ControlState _state = ControlState::None);

		// 获取上边框宽度
		int_t GetBorderTop(ControlState _state = ControlState::None);

		// 获取右边框宽度
		int_t GetBorderRight(ControlState _state = ControlState::None);

		// 获取下边框宽度
		int_t GetBorderBottom(ControlState _state = ControlState::None);

		// 获取边框颜色
		Color GetBorderColor(ControlState _state = ControlState::None);

		// 获取前景图片
		Image* GetForeImage(ControlState _state = ControlState::None);

		// 获取背景图片
		Image* GetBackImage(ControlState _state = ControlState::None);

		// 获取背景颜色
		Color GetBackColor(ControlState _state = ControlState::None);

		// 获取旋转角度
		float GetAngle(ControlState _state = ControlState::None);

		// 获取当前控件的鼠标光标（可继承）
		virtual HCURSOR GetHCursor();

		// 获取前景颜色
		Color GetForeColor(ControlState _state = ControlState::None);

		// 获取字体 Family
		std::wstring GetFontFamily(ControlState _state = ControlState::None);

		// 获取字体大小
		int_t GetFontSize(ControlState _state = ControlState::None);

	public:
		// 构造函数
		Control();

		// 析构函数
		virtual ~Control();

		// 销毁控件内所有弹簧
		void DestroySpacers();

		// 以下函数请保证在父控件布局已完成的情况下使用，使用 ResumeLayout() 执行布局
		// 获取 X 坐标
		int_t X();

		// 获取 Y 坐标
		int_t Y();

		// 获取宽度
		int_t Width();

		// 获取高度
		int_t Height();

		// 设置 X 坐标
		void SetX(int_t X);

		// 设置 Y 坐标
		void SetY(int_t Y);

		// 移动相对于父控件的位置
		void SetLocation(const Point& pt);

		// 设置控件大小（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetSize(const Size& size);

		// 设置绝对宽高
		void SetFixedSize(const Size& size);

		// 设置宽度（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetWidth(int_t width);

		// 设置高度（当重绘控件时不建议多次使用，影响性能，会调用 SetRect 函数）
		void SetHeight(int_t height);

		// 设置绝对宽度
		void SetFixedWidth(int_t fixedWidth);

		// 设置绝对高度
		void SetFixedHeight(int_t fixedHeight);

		// 设置相对父控件矩形，返回实际的 rect
		const Rect& SetRect(const Rect& rect);

		// 获取绝对宽度
		int_t GetFixedWidth();

		// 获取绝对高度
		int_t GetFixedHeight();

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
		virtual const Size& GetContentSize();

		// 获取控件大小
		Size GetSize();

		// 获取控件位置
		Point GetLocation();

		// 获取相对于父控件的矩形（布局计算后）
		virtual const Rect& GetRect();
		// 获取基于客户端区域的矩形
		Rect GetClientRect();

		// 获取控件在窗口中可视区域的矩形
		const Rect& GetViewRect();

		// 获取控件的 DockStyle（停靠方式）
		DockStyle GetDockStyle();

		// 设置控件的 DockStyle
		void SetDockStyle(const DockStyle& dockStyle);

		// 获取控件的缩放系数
		float GetScale();

		// 是否存在挂起的布局
		bool IsPendLayout();

		// 尝试挂起布局，返回当前布局状态
		const LayoutState TryPendLayout();

		// 获取当前布局状态
		const LayoutState GetLayoutState();

		// 结束当前布局（使其立即生效）
		void EndLayout();

		// 立即强制刷新布局
		virtual void RefreshLayout();

		// 设置提示文字（类似 tooltip）
		void SetTips(const EString& text);

		// 获取提示文字
		const EString& GetTips();

		// 获取控件的滚动条对象
		virtual ScrollBar* GetScrollBar();

		// 派发事件（如鼠标单击事件等...）返回true则事件成功派发 返回false代表当前控件对象已被释放
		bool SendNotify(const EventArgs& arg);

		// 设置控件属性
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);

		// 获取当前可见的子控件集合
		const Controls& GetViewControls();

		// 获取所有子控件（不建议直接修改）
		const Controls& GetControls();

		// 使用下标获取控件，自动跳过 spacer 类控件
		Control* GetControl(int_t pos);

		// 是否包含指定控件（递归遍历所有子控件）
		bool Contains(Control* ctl);

		// 获取指定子控件的索引
		int_t IndexOf(Control* childCtl);

		// 根据 name 查找控件（包括自身）
		Control* FindControl(const EString& ctlName);

		// 根据属性查找所有匹配控件（包括自身）
		Controls FindControl(const EString& attrName, const EString& attrValue);

		// 根据属性查找第一个匹配控件（包括自身）
		Control* FindSingleControl(const EString& attrName, const EString& attrValue);

		// 根据 name 查找子控件（仅限直接子集）
		Control* FindChild(const EString& ctlName);

		// 根据属性查找所有匹配的子控件（仅限直接子集）
		Controls FindChild(const EString& attrName, const EString& attrValue);

		// 根据属性查找第一个匹配的子控件（仅限直接子集）
		Control* FindSingleChild(const EString& attrName, const EString& attrValue);

		// 交换两个子控件的位置
		virtual bool SwapChild(Control* childCtl, Control* childCt2);

		// 在指定位置插入子控件
		virtual void Insert(int_t pos, Control* childCtl);

		// 添加控件到末尾（如果是弹簧控件，在释放时将自动销毁）
		virtual Control* Add(Control* childCtl);

		// 移除控件，freeCtrl 标志是否释放控件内存
		virtual void Remove(Control* childCtl, bool freeCtrl = false);

		// 设置控件的父控件
		virtual void SetParent(Control* parentCtl);

		// 清空所有子控件
		virtual void Clear();

		// 清空所有子控件，freeChilds 决定是否释放子控件内存
		virtual void Clear(bool freeChilds);

		// 设置控件可见性
		virtual void SetVisible(bool flag);

		// 获取控件可见性状态
		virtual bool IsVisible();

		// 标记控件区域为无效（将会延迟刷新UI）
		virtual bool Invalidate();

		// 立即强制刷新控件区域并更新无效区域（且立即触发布局）
		virtual void Refresh();

	};

	//内联页面Interface类
	class UI_EXPORT IIFrame :public Control {
	public:
		IIFrame() {};
		virtual ~IIFrame() {};
	};

	//添加弹簧无需用户手动释放,
	class UI_EXPORT Spacer :public Control {
	public:
		virtual ~Spacer() {};
	};
	//具有绝对高度的 的弹簧
	class UI_EXPORT VSpacer :public Spacer {
	private:
		VSpacer() {};
	public:
		virtual ~VSpacer() {};
		VSpacer(int_t fixedHeight) {
			SetFixedHeight(fixedHeight);
		}
	};
	//具有绝对宽度的 的弹簧
	class UI_EXPORT HSpacer :public Spacer {
	private:
		HSpacer() {};
	public:
		virtual ~HSpacer() {};
		HSpacer(int_t fixedWidth) {
			SetFixedWidth(fixedWidth);
		}
	};

	class UI_EXPORT ScrollBar :public Control {
	protected:
		//鼠标是否已经按下
		bool _mouseDown = false;
		//上一次鼠标命中的坐标
		int_t _lastPoint = 0;
		//滚动条当前的坐标
		double _sliderPos = 0;
		//滚动条的长度
		int_t _sliderLength = 0;
		//滚动条每滚动一次的比率
		double _rollRate = 0;
		//父容器内的坐标偏移
		int_t _offset = 0;
		//父容器的内容长度
		int_t _contentLength = 0;
		//父容器可见长度(容器自身长度)
		int_t _viewLength = 0;
		//溢出容器的长度
		int_t _overflowLength = 0;

		//int_t _old_viewLength = 0;
		//int_t _old_contentLength = 0;
		//int_t _old_offset = 0;
	public:
		//滚动条计算出偏移之后的回调函数
		std::function<void(int_t)> OffsetCallback = NULL;
		//滚动事件 arg1:发送者 arg2:滚动百分比 arg3:滚动类型
		std::function<void(ScrollBar*, float, Event)> Scroll = NULL;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnMouseLeave(const MouseEventArgs& arg) override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void GetInfo(int_t* viewLength, int_t* contentLength, int_t* scrollBarLength) = 0;
		void ScrollTo(int_t offset, const Event& type);
		void SyncInfo();
	public:
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctl) = 0;
		//按照百分比滚动 0.0f~1.0f
		void ScrollTo(float scrollRate);
		//获取当前滚动到的位置 进度的百分比
		float ScrollPos();
		//获取滑块的矩形
		virtual Rect GetSliderRect() = 0;//
		virtual void ParentSize(const Size& parentSize) = 0;
		//滚动条是否已经绘制且显示
		bool IsDraw();
		//重置滚动条数据到起点(不执行重绘)
		void Reset();
		//滚动条是否能够滚动
		bool Scrollable();
		//当父控件发生内容发生改变 请调用刷新滚动条
		void RefreshScroll();
		ScrollBar();
		virtual ~ScrollBar();
	};
};