#pragma once
#include "EzUI.h"
namespace EzUI {
	class UI_EXPORT Control :public IControl
	{
	private:
		bool* _isRemove = NULL;//控件是否已经被移除或释放
		bool _visible = true;//控件是否可见 此标志为true的时候 可能实际中并不会可见 
		float _scale = 1.0f;//当前控件的dpi缩放
		std::list<Control*> _controls;//子控件
		std::list<Control*> _spacers;//存储弹簧控件
		//布局状态AddControl丶InsertControl丶RemoveControl丶OnSize时候此标志为挂起 调用ResumeLayout标志为布局中 当调用OnLayout()之后此标志为None
		EzUI::LayoutState _layoutState = EzUI::LayoutState::None;
		EString _tipsText;//鼠标悬浮的提示文字
		Point _lastLocation;//上一次位置
		Size _lastSize;//上一次大小
		bool _autoWidth = false;//是否根据内容自动宽度
		bool _autoHeight = false;//根据内容的高度自动变化
		Size _contentSize;//控件内容宽高
		Size _fixedSize;//绝对Size
		Rect _rect;//控件矩形区域(基于父控件)
		Rect _viewRect;//控件在窗口中的可见区域
		DockStyle _dock = DockStyle::None;//dock样式
	private:
		Control(const Control&) = delete;
		Control& operator=(const Control&) = delete;
		void ComputeClipRect();//计算基于父控件的裁剪区域
	protected:
		std::list<Control*> ViewControls;//基于控件中的可见控件
	public:
		Distance Margin;//外边距 让容器独占一行 或 一列的情况下 设置边距会使控件变小 不可设置为负数
		//添加到主窗口通知函数中可拦截
		Event EventNotify = Event::OnMouseClick | Event::OnMouseDoubleClick | Event::OnMouseWheel | Event::OnMouseEnter | Event::OnMouseMove | Event::OnMouseDown | Event::OnMouseUp | Event::OnMouseLeave | Event::OnKeyChar | Event::OnKeyDown | Event::OnKeyUp;
		Event EventPassThrough = Event::None;//控件可被穿透的事件
		bool Enable = true;//控件被启用 禁止状态下鼠标键盘消息将不可用
		EString Name;//控件的ObjectName ID
		ControlState State = ControlState::Static;//控件状态
		ControlAction Action = ControlAction::None;//控件行为
		ControlStyle Style;//默认样式
		ControlStyle HoverStyle;//鼠标悬浮样式
		ControlStyle ActiveStyle;//鼠标按下样式
		Control* Parent = NULL;//父控件
		std::function<void(Control*, const EventArgs&)> EventHandler = NULL;//事件处理器
	protected:
		//仅限子类使用
		virtual void SetContentWidth(const int& width);//
		virtual void SetContentHeight(const int& height);//
		virtual void SetContentSize(const Size& size);//
		virtual bool OnEvent(const EventArgs& arg);//所有事件先进这里
		virtual void OnPaintBefore(PaintEventArgs& args, bool paintSelf = true);//绘制之前
		virtual void OnPaint(PaintEventArgs& args);//绘制 
		virtual void OnChildPaint(PaintEventArgs& args);//子控件绘制 可以重载此函数优化鼠标操作性能
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//背景绘制
		virtual void OnForePaint(PaintEventArgs& e);//前景绘制
		virtual void OnBorderPaint(PaintEventArgs& painter, const Border& border);//边框绘制
		virtual void OnLocation(const LocationEventArgs& arg);//坐标发生改变
		virtual void OnSize(const SizeEventArgs& arg);//大小发生改变
		virtual void OnRect(const RectEventArgs& arg);
		virtual void OnDpiChange(const DpiChangeEventArgs& arg);
		//布局代码在此 需要重写布局请重写此函数 
		virtual void OnLayout();
		//鼠标事件
		virtual void OnMouseMove(const MouseEventArgs& arg);//鼠标在控件上移动
		virtual void OnMouseLeave(const MouseEventArgs& args);//鼠标离开控件
		virtual void OnMouseWheel(const MouseEventArgs& arg);//鼠标滚轮
		virtual void OnMouseDown(const MouseEventArgs& arg);//鼠标按下
		virtual void OnMouseUp(const MouseEventArgs& arg);//鼠标弹起
		virtual void OnMouseClick(const MouseEventArgs& arg);//鼠标单击
		virtual void OnMouseDoubleClick(const MouseEventArgs& arg);//鼠标双击
		virtual void OnMouseEnter(const MouseEventArgs& arg);//鼠标移入
		virtual void OnMouseEvent(const MouseEventArgs& args);//鼠标事件消息
		//键盘事件
		virtual void OnKeyBoardEvent(const KeyboardEventArgs& _args);//键盘事件消息
		virtual void OnKeyChar(const KeyboardEventArgs& _args);//WM_CAHR消息
		virtual void OnKeyDown(const KeyboardEventArgs& _args);//WM_CAHR消息
		virtual void OnKeyUp(const KeyboardEventArgs& _args);//键盘弹起
		//失去焦点
		virtual void OnKillFocus(const KillFocusEventArgs& _args);//失去焦点的时候发生
		virtual void OnRemove();//被移除该做的事情
	public:
		virtual ControlStyle& GetStyle(const ControlState& _state);//获取当前控件状态下的样式信息
		virtual ControlStyle& GetDefaultStyle();//用于获取不同控件当前默认的
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
		float GetAngle(ControlState _state = ControlState::None);
		//具有继承性样式
		virtual HCURSOR GetHCursor();
		Color GetForeColor(ControlState _state = ControlState::None);//获取默认控件状态下前景色
		std::wstring GetFontFamily(ControlState _state = ControlState::None);//获取默认控件状态下字体Family
		int GetFontSize(ControlState _state = ControlState::None);//获取默认控件状态下字体大小样式
	public:
		Control();
		virtual ~Control();
		void DestroySpacers();//销毁控件内所有弹簧
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
		const Rect& SetRect(const Rect& rect);//设置相对父控件矩形 返回实际的rect
		const int& GetFixedWidth();//获取绝对宽度
		const int& GetFixedHeight();//获取绝对高度
		virtual Rect GetCareRect();//获取光标位置
		virtual bool IsAutoWidth();//是否自动高度
		virtual bool IsAutoHeight();//是否自动高度
		virtual void SetAutoWidth(bool flag);//设置自动宽度
		virtual void SetAutoHeight(bool flag);//设置自动高度
		virtual void SetAutoSize(bool flag);
		virtual const Size& GetContentSize();
		Size GetSize();
		Point GetLocation();
		virtual const Rect& GetRect();//获取相对与父控件矩形 布局计算后
		Rect GetClientRect();//获取基于客户端的区域
		const Rect& GetViewRect();//获取基于窗口中的可视区域
		const DockStyle& GetDockStyle();//获取dock标志
		void SetDockStyle(const DockStyle& dockStyle);
		const float& GetScale();
		bool IsPendLayout();//是否含有挂起的布局
		const LayoutState& TryPendLayout();//尝试挂起布局 返回当前布局状态
		const LayoutState& GetLayoutState();//获取当前布局状态
		void EndLayout();//结束布局
		virtual void RefreshLayout();//刷新布局
		void SetTips(const EString& text);//设置tips文字
		const EString& GetTips();//获取tips文字
		virtual ScrollBar* GetScrollBar();//获取控件的滚动条
		bool DispatchEvent(const EventArgs& arg);//派发失去焦点事件
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::Static);//设置样式类似qt那样
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//基础控件设置属性
		const std::list<Control*>& GetViewControls();//获取当前可见控件
		const std::list<Control*>& GetControls();//获取当前所有子控件 const修饰是因为不建议直接修改子控件内容
		Control* GetControl(size_t pos);//使用下标获取控件 会自动过滤spacer(弹簧)这类的控件
		bool Contains(Control* ctl);//会递归循全部包含的控件是否存在
		size_t IndexOf(Control* childCtl);//获取子控件在此容器中的索引
		Control* FindControl(const EString& ctlName);//使用name寻找容器中控件,包括自身
		std::vector<Control*> FindControl(const EString& attrName, const EString& attrValue);//使用属性寻找容器中符合条件的控件,包括自身
		Control* FindSingleControl(const EString& attrName, const EString& attrValue);//使用属性寻找容器中第一个符合条件的控件,包括自身
		Control* FindChild(const EString& ctlName);//寻找控件,仅限子集
		std::vector<Control*> FindChild(const EString& attrName, const EString& attrValue);//寻找控件,仅限子集
		Control* FindSingleChild(const EString& attrName, const EString& attrValue);//寻找控件,仅限子集
		virtual bool SwapChild(Control* childCtl, Control* childCt2);//对子控件的两个控件进行位置交换
		virtual void Insert(size_t pos, Control* childCtl);//选择性插入控件
		virtual void Add(Control* childCtl);//添加控件到末尾
		virtual void Remove(Control* childCtl);//删除控件 返回下一个迭代器
		virtual void SetParent(Control* parentCtl);//设置父控件
		virtual void Clear();//清空当前所有子控件
		virtual void Clear(bool freeChilds);//清空当前所有子控件, freeControls是否释放所有子控件
		virtual void SetVisible(bool flag);//设置Visible标志
		virtual bool IsVisible();//获取Visible标志
		virtual bool Invalidate();// 使当前控件的区域为无效区域
		virtual void Refresh();//使当前控件区域为无效区域并且立即更新全部的无效区域(更新时会对正在挂起的布局立即生效,直到无效区域更新完毕)
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
		VSpacer(int fixedHeight) {
			SetFixedHeight(fixedHeight);
		}
	};
	//具有绝对宽度的 的弹簧
	class UI_EXPORT HSpacer :public Spacer {
	private:
		HSpacer() {};
	public:
		virtual ~HSpacer() {};
		HSpacer(int fixedWidth) {
			SetFixedWidth(fixedWidth);
		}
	};

	class UI_EXPORT ScrollBar :public Control {
	protected:
		//鼠标是否已经按下
		bool _mouseDown = false;
		//上一次鼠标命中的坐标
		int _lastPoint = 0;
		//滚动条当前的坐标
		double _sliderPos = 0;
		//滚动条的长度
		int _sliderLength = 0;
		//滚动条每滚动一次的比率
		double _rollRate = 0;
		//父容器内的坐标偏移
		int _offset = 0;
		//父容器的内容长度
		int _contentLength = 0;
		//父容器可见长度(容器自身长度)
		int _viewLength = 0;
		//溢出容器的长度
		int _overflowLength = 0;

		//int _old_viewLength = 0;
		//int _old_contentLength = 0;
		//int _old_offset = 0;
	public:
		//滚动条计算出偏移之后的回调函数
		std::function<void(int)> OffsetCallback = NULL;
		//滚动事件 arg1:发送者 arg2:滚动百分比 arg3:滚动类型
		std::function<void(ScrollBar*, float, Event)> Scroll = NULL;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnMouseLeave(const MouseEventArgs& arg) override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength) = 0;
		void ScrollTo(int offset, const Event& type);
		void SyncInfo();
	public:
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctl) = 0;
		//按照百分比滚动 0.0f~1.0f
		void ScrollTo(const float& scrollRate);
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