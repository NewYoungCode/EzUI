#pragma once
#include "EzUI.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		//ControlStyle _nowStyle;//临时组成的样式
		bool* _isRemove = NULL;
		bool _stateRepaint = false;//状态发生改变的时候绘制
		bool _mouseIn = false;//鼠标是否在控件内
		bool _visible = true;//控件是否可见 此标志为true的时候 可能实际中并不会可见 
		Controls _controls;//子控件
		//布局状态AddControl丶RemoveControl丶OnSize时候此标志为挂起 调用ResumeLayout标志为布局中 当调用OnLayout()之后此标志为None
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;
		std::wstring _tipsText;//鼠标悬浮的提示文字
		Point _lastLocation;//上一次大小
		Size _lastSize;//上一次大小
		Rect _lastDrawRect;//最后一次显示的位置
		int _eventNotify = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnKeyChar | Event::OnKeyDown | Event::OnKeyUp;//默认添加到主窗口通知函数中可拦截
		std::mutex _rePaintMtx;//避免多线程中调用Invalidate()的问题
		Control(const Control&) = delete;
		Control& operator=(const Control&) = delete;
		bool IsRePaint();//是否需要重绘
		bool CheckEventPassThrough(const Event& eventType);//检查事件是否已经过滤
		bool CheckEventNotify(const Event& eventType);//检查事件是否通知到主窗口中
		void ComputeClipRect();//计算基于父控件的裁剪区域
		Size _fixedSize{ 0,0 };//绝对Size
		Rect _rect;//控件矩形区域(基于父控件)
		DockStyle _dock = DockStyle::None;//dock样式
	public:
		EzUI::Margin Margin;//外边距 让容器独占一行 或 一列的情况下 设置边距会使控件变小 不可设置为负数
		WindowData* PublicData = NULL;//窗口上的公共数据
		int MousePassThrough = 0;//忽略的鼠标消息
		bool Enable = true;//控件被启用 禁止状态下鼠标键盘消息将不可用
		EString Name;//控件的ObjectName ID
		ControlState State = ControlState::Static;//控件状态
		ControlAction Action = ControlAction::None;//控件行为
		ControlStyle Style;//默认样式
		ControlStyle HoverStyle;//鼠标悬浮样式
		ControlStyle ActiveStyle;//鼠标按下样式
		Control* Parent = NULL;//父控件
		Controls VisibleControls;//基于控件中的可见控件
		const Rect ClipRect;//控件在窗口中的可见区域
	public:
		EventMouseMove MouseMove;//移动事件
		EventMouseEnter MouseEnter;//移入事件
		EventMouseWheel MouseWheel;//滚轮事件
		EventMouseLeave MouseLeave;//鼠标离开事件
		EventMouseDown MouseDown;//鼠标按下事件
		EventMouseUp MouseUp;//鼠标抬起
		EventMouseClick MouseClick;//鼠标单击
		EventMouseDoubleClick MouseDoubleClick;//鼠标双击
		EventKeyChar KeyChar;//WM_CHAR消息
		EventKeyDown KeyDown;//键盘按下
		EventKeyUp KeyUp;//键盘弹起
		EventPaint Painting;//绘制事件
	protected:
		virtual ControlStyle& GetStyle(const ControlState& _state);//获取当前控件状态下的样式信息
		virtual ControlStyle& GetDefaultStyle();//用于获取不同控件当前默认的
		virtual void OnPaint(PaintEventArgs& args);//绘制 
		virtual void OnChildPaint(PaintEventArgs& args);//子控件绘制 可以重载此函数优化鼠标操作性能
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//背景绘制
		virtual void OnForePaint(PaintEventArgs& e);//前景绘制
		virtual void OnBorderPaint(PaintEventArgs& painter, const Border& border);//边框绘制
		virtual void OnLocation(const Point& pt);//坐标发生改变
		virtual void OnSize(const Size& size) override;//大小发生改变
		virtual void OnRect(const Rect& rect) override;
		virtual void OnLayout();//布局代码在此 布局完成之后PendLayout设置成false
		virtual bool OnKeyBoardEvent(const KeyboardEventArgs& _args);//键盘事件消息
		virtual void OnMouseMove(const Point& point);//鼠标在控件上移动
		virtual void OnMouseLeave();//鼠标离开控件
		virtual void OnMouseWheel(int _rollCount, short zDelta, const Point& point);//鼠标滚轮
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//鼠标按下
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//鼠标弹起
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//鼠标单击
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//鼠标双击
		virtual void OnMouseEnter(const Point& point);//鼠标移入
		virtual bool OnMouseEvent(const MouseEventArgs& args);//鼠标事件消息
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam) override;//WM_CAHR消息
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) override;//WM_CAHR消息
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起
		virtual void OnKillFocus(Control* control);//失去焦点的时候发生
		virtual void Rending(PaintEventArgs& args);//绘制函数
	public:
		//以下函数请保证在父控件布局已完成的情况下使用 使用ResumeLayout()执行布局
		const int& X();
		const int& Y();
		const int& Width();
		const int& Height();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//移动相对与父控件的位置
		void SetSize(const Size& size); //当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetFixedSize(const Size& size); //设置绝对宽高
		void SetWidth(const int& width);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetHeight(const int& height);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetFixedWidth(const int& fixedWidth);//设置绝对宽度
		void SetFixedHeight(const int& fixedHeight);//设置绝对高度
		const int& GetFixedWidth();//获取绝对宽度
		const int& GetFixedHeight();//获取绝对高度
		virtual const Rect& GetRect();//获取相对与父控件矩形 布局计算后
		Rect GetClientRect();//获取基于客户端的矩形
		const DockStyle& GetDockStyle();//获取dock标志
		void SetDockStyle(const DockStyle& dockStyle);
		bool IsPendLayout();//是否含有挂起的布局
		const LayoutState& TryPendLayout();//尝试挂起布局 返回当前布局状态
		void EndLayout();//结束布局
		const Rect& SetRect(const Rect& rect);//设置相对父控件矩形 返回实际的rect
		virtual void ResumeLayout();//直接进行布局
		virtual void SetTips(const EString& text);//设置tips
		virtual void OnRemove();//被移除该做的事情
		virtual bool DispatchEvent(const EventArgs& arg);//用于消息事件的函数
		void AddEventNotify(int eventType);//添加到主窗口Ontify函数中可拦截
		void RemoveEventNotify(int eventType);//移除一个主窗口的Ontify消息
		virtual ScrollBar* GetScrollBar();//获取控件的滚动条
	public:
		//普通样式
		int GetBorderTopLeftRadius(ControlState _state = ControlState::None);
		int GetBorderTopRightRadius(ControlState _state = ControlState::None);
		int GetBorderBottomRightRadius(ControlState _state = ControlState::None);
		int GetBorderBottomLeftRadius(ControlState _state = ControlState::None);
		int GetBorderLeft(ControlState _state = ControlState::None);
		int GetBorderTop(ControlState _state = ControlState::None);
		int GetBorderRight(ControlState _state = ControlState::None);
		int GetBorderBottom(ControlState _state = ControlState::None);
		Color GetBorderColor(ControlState _state = ControlState::None);
		Image* GetForeImage(ControlState _state = ControlState::None);
		Image* GetBackImage(ControlState _state = ControlState::None);
		Color GetBackColor(ControlState _state = ControlState::None);
		//具有继承性样式
		Color GetForeColor(ControlState _state = ControlState::None);//获取默认控件状态下前景色
		std::wstring GetFontFamily(ControlState _state = ControlState::None);//获取默认控件状态下字体Family
		int GetFontSize(ControlState _state = ControlState::None);//获取默认控件状态下字体大小样式
	public:
		Control();
		virtual ~Control();
		void DestroySpacers();//销毁控件内所有弹簧
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::Static);//
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//基础控件设置属性
		const Controls& GetControls();//获取当前所有子控件
		bool Contains(Control* ctl);//会递归循全部包含的控件是否存在
		Control* FindControl(size_t pos);//使用下标获取控件
		Control* FindControl(const EString& objectName);//寻找子控件 包含孙子 曾孙 等等
		Controls FindControl(const EString& attr, const EString& attrValue);//使用属性查找
		bool Swap(Control* ct1, Control* ct2);//对子控件的两个控件进行位置交换

		size_t Index();//获取当前控件在父容器下的索引
		virtual void AddControl(Control* ctl);//添加控件
		virtual void RemoveControl(Control* ctl);//删除控件 返回下一个迭代器
		virtual void Clear(bool freeControls = false);//清空当前所有子控件, freeControls是否释放所有子控件
		
		virtual void SetVisible(bool flag);//设置Visible标志
		bool IsVisible();//获取Visible标志
		virtual bool IsInWindow();//当前是否显示在窗口内 代表实际情况是否显示
		virtual bool Invalidate();// 使当前控件的区域为无效区域
		virtual void Refresh();// 使当前控件区域为无效区域并且立即更新全部的无效区域
		virtual Rect GetCareRect();//获取光标位置
		virtual bool IsAutoWidth();//是否自动高度
		virtual bool IsAutoHeight();//是否自动高度
		virtual void SetAutoWidth(bool flag);//设置自动宽度
		virtual void SetAutoHeight(bool flag);//设置自动高度
	};

	//添加弹簧无需用户手动释放,
	class Spacer :public Control {
	private:
		//弹簧不允许再出现子控件
		void AddControl(Control* ctl) {};
		ControlIterator RemoveControl() {};
	public:
		virtual ~Spacer() {};
	};
	//具有绝对高度的 的弹簧
	class VSpacer :public Spacer {
	private:
		VSpacer() {};
	public:
		virtual ~VSpacer() {};
		VSpacer(int fixedHeight) {
			SetFixedHeight(fixedHeight);
		}
	};
	//具有绝对宽度的 的弹簧
	class HSpacer :public Spacer {
	private:
		HSpacer() {};
	public:
		virtual ~HSpacer() {};
		HSpacer(int fixedWidth) {
			SetFixedWidth(fixedWidth);
		}
	};
	//
	// 摘要:
	// 为支持自动滚动行为的控件定义一个基类。
	class ScrollableControl :public Control {
	protected:
		//上下文宽度
		int _contentWidth = 0;
		//上下文高度
		int _contentHeight = 0;
	public:
		ScrollableControl() {};
		const int& GetContentWidth() { return _contentWidth; };
		const int& GetContentHeight() { return _contentHeight; };
		virtual ~ScrollableControl() {};
	};
	class ScrollBar :public Control {
	private:
		//滚动条不允许再出现子控件
		void AddControl(Control* ctl) {};
		ControlIterator RemoveControl() {};
	protected:
		//鼠标是否已经按下
		bool _mouseDown = false;
		//上一次鼠标命中的坐标
		int  _lastPoint = 0;
		// 滚动条当前的坐标
		double _sliderPos = 0;
		// 滚动条的长度
		int _sliderLength = 0;
	public:
		//滚动条所属者 当所属者被移除或者被释放掉 请注意将此指针置零
		ScrollableControl* OWner = NULL;
		//滚动条计算出偏移之后的回调函数
		std::function<void(int)> OffsetCallback = NULL;
		EventScrollRolling Rolling = NULL;//滚动事件
	protected:
		virtual void OnMouseUp(MouseButton mBtn, const Point& point)override
		{
			__super::OnMouseUp(mBtn, point);
			_mouseDown = false;
		}
		virtual void OnMouseLeave() override
		{
			__super::OnMouseLeave();
			_mouseDown = false;
		}
	public:
		virtual void Move(double pos) = 0;
		virtual Rect GetSliderRect() = 0;//
		virtual void OWnerSize(const Size& parentSize) = 0;
		//滚动条是否已经绘制且显示
		virtual bool IsDraw() = 0;
		//滚动条是否能够滚动
		virtual bool Scrollable() = 0;
		//当OWner发生内容发生改变 请调用刷新滚动条
		void RefreshScroll() {
			Move(_sliderPos);
		};
		ScrollBar() {
			Style.ForeColor = Color(205, 205, 205);//the bar default backgroundcolor
			SetSize({ 10,10 });
		}
		virtual ~ScrollBar() {}
	};
};