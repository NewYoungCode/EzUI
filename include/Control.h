#pragma once
#include "Application.h"
#include "IControl.h"
#include "Painter.h"
class Control;
class ScrollBar;
class Spacer;
typedef std::list<Control*> Controls;
typedef std::list<Control*>::iterator ControlIterator;
#define UIFunc std::function
#define EventMouseMove  UIFunc<void(Control*,const Point&)>  //移动事件
#define EventMouseEnter  UIFunc<void( Control*, const Point&)>//移入事件
#define EventMouseWheel   UIFunc<void( Control*, short, const Point&)>//滚轮事件
#define EventMouseLeave  UIFunc<void( Control*)>//鼠标离开事件
#define EventMouseDown  UIFunc<void( Control*, MouseButton, const Point&)> //鼠标按下事件
#define EventMouseUp  UIFunc<void ( Control*, MouseButton, const Point&)>//鼠标抬起
#define EventMouseClick   UIFunc<void( Control*, MouseButton, const Point&)>//鼠标单击
#define EventMouseDoubleClick   UIFunc<void( Control*, MouseButton, const Point&)>//鼠标双击

class UI_EXPORT Control :public IControl
{
private:
	Control(const Control&);
	Control& operator=(const Control&);
protected:
	int _anchorStyle = (AnchorStyle::Top | AnchorStyle::Left);
	Rect _rect;
	Controls _controls;
	int _fixedWidth = 0;
	int _fixedHeight = 0;
	bool _isAnchorStyle = false;
	int _right = 0;
	int _bottom = 0;
	int _marginRight = 0;
	int _marginBottom = 0;
	Tuple<LPCSTR> _LastCursor;
	Controls _spacer;//存储控件下布局的的弹簧集合
public:
	const Rect ClipRect;//控件在窗口中的可见区域
	EString Name;//控件的ObjectName
	ScrollBar* ScrollBar = NULL;//垂直滚动条或者水平滚动条 一个控件只允许有一个
	ControlState State = ControlState::None;//控件状态
	ControlAction Action = ControlAction::None;//控件行为
	ControlStyle Style;//默认样式
	ControlStyle HoverStyle;//鼠标悬浮样式
	ControlStyle ActiveStyle;//鼠标按下样式
	DockStyle Dock = DockStyle::None;//dock样式
	bool Visible = true;//控件是否可见
	Control* Parent = NULL;//父控件
	Controls VisibleControls;//基于控件中的可见控件
	//鼠标事件相关
	EventMouseMove MouseMove;//移动事件
	EventMouseEnter MouseEnter;//移入事件
	EventMouseWheel MouseWheel;//滚轮事件
	EventMouseLeave MouseLeave;//鼠标离开事件
	EventMouseDown MouseDown;//鼠标按下事件
	EventMouseUp MouseUp;//鼠标抬起
	EventMouseClick MouseClick;//鼠标单击
	EventMouseDoubleClick MouseDoubleClick;//鼠标双击
	Tuple<LPCTSTR> Cursor;//鼠标样式  默认 IDC_ARROW
public:
	const ControlType& GetType();
	virtual void OnChar(WPARAM wParam, LPARAM lParam) override;//WM_CAHR消息
	virtual void OnKeyDown(WPARAM wParam) override;//WM_CAHR消息
	virtual void OnPaint(PaintEventArgs& args);//绘制 
	virtual void OnChildPaint(Controls& controls, PaintEventArgs& args);//子控件绘制 可以重载此函数优化鼠标操作性能
	virtual void OnBackgroundPaint(PaintEventArgs& painter);//背景绘制
	virtual void OnForePaint(PaintEventArgs& e);//前景绘制
	virtual void OnBorderPaint(PaintEventArgs& painter);//边框绘制
	void OnMouseEvent(MouseEventArgs& args);//鼠标事件消息
	void OnEvent(Event eventType, void* param);//基础事件消息
	virtual void OnLayout(const Size& parentRect, bool instantly = true);//父控件大小改变事件  instantly立即生效
	virtual void OnLoad();//控件第一次加载 警告 此函数在LayerWindow里面不允许在函数内添加控件 但是允许设置控件参数  
	virtual void OnSize(const Size& size);//大小发生改变
public:
	virtual void OnMouseMove(const Point& point);//鼠标在控件上移动
	virtual void OnMouseLeave();//鼠标离开控件
	virtual void OnMouseWheel(short zDelta, const Point& point);//鼠标滚轮
	virtual void OnMouseDown(MouseButton mbtn, const Point& point);//鼠标按下
	virtual void OnMouseUp(MouseButton mbtn, const Point& point);//鼠标弹起
	virtual void OnMouseClick(MouseButton mbtn, const Point& point);
	virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//鼠标双击
	//鼠标单击
	virtual void OnMouseEnter(const Point& point);//已实现
protected:
	ControlStyle& GetStyle(ControlState _state);//获取当前控件状态下的样式信息
public:
	Control();
	virtual ~Control();
	void DestroySpacers();
	//普通样式
	HImage  GetForeImage();
	HImage  GetBackgroundImage();
	UI_Int GetRadius();
	UI_Int GetBorderLeft();
	UI_Int GetBorderTop();
	UI_Int GetBorderRight();
	UI_Int GetBorderBottom();
	Color GetBorderColor();
	Color GetBackgroundColor();
	//具有继承性样式
	EString GetFontFamily(ControlState _state = ControlState::None);//获取默认控件状态下字体Family
	UI_Float GetFontSize(ControlState _state = ControlState::None);//获取默认控件状态下字体大小样式
	Color GetForeColor(ControlState _state = ControlState::None);//获取默认控件状态下前景色
	Control* FindControl(const EString& objectName);//寻找子控件 包含孙子 曾孙 等等
	Control* FindControl(Control* ctl);//使用指针寻找子控件 不包含孙子 曾孙 
	void SetAnchorStyle(int anchorStyle);//设置控件对齐方式
	int  GetAnchorStyle();//获取锚定风格
	Controls* GetControls();//获取当前所有子控件
	virtual void AddControl(Control* ctl);//添加控件
	virtual ControlIterator RemoveControl(Control* ctl);//删除控件 返回下一个迭代器
	void Clear(bool freeControls = false);//清空当前所有子控件, freeControls是否释放所有子控件
	void Move(const Point& pt);//移动相对与父控件的位置
	const int& X();
	const int& Y();
	void SetX(int X);
	void SetY(int Y);
	void ReSize(const Size& size); //当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
	void SetWidth(int width);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
	void SetHeight(int height);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
	const int& Width();
	const int& Height();
	void ComputeClipRect();//计算基于父控件的裁剪区域
	void SetFixedWidth(int fixedWidth);//设置绝对宽度
	void SetFixedHeight(int fixedHeight);//设置绝对高度
	const int& GetFixedWidth();//获取绝对宽度
	const int& GetFixedHeight();//获取绝对高度
	virtual void Refresh();// 刷新当前控件
	Rect GetClientRect();//获取基于客户端的矩形
	virtual void SetRect(const Rect& rect, bool rePaint = false);//设置相对父控件矩形
	const Rect& GetRect();//获取相对与父控件矩形
};
//添加弹簧无需用户手动释放,
class Spacer :public Control {
public:
	Spacer() :Control() {
		//this->Visible = false;
		this->_Type = ControlType::ControlSpacer;
	}
	virtual ~Spacer() {};
public:
#ifdef DEBUGPAINT
	void OnPaint(Painter& pt, PaintEventArgs& args) {
		pt.DrawString("auto", GetFontFamily(this->State), 8, GetForeColor(this->State), RectF(0, 0, (float)_rect.Width, (float)_rect.Height), TextAlign::MiddleCenter);
	}
#endif
};

//具有绝对高度的 的弹簧
class VSpacer :public Spacer {
private:
	VSpacer() {};
public:
	VSpacer(int fixedHeight) {
		SetFixedHeight(fixedHeight);
	}
#ifdef DEBUGPAINT
	void OnPaint(Painter& pt, PaintEventArgs& args) {
		pt.DrawString(std::to_string(GetFixedHeight()), GetFontFamily(this->State), 8, GetForeColor(this->State), RectF(0, 0, (float)_rect.Width, (float)_rect.Height), TextAlign::MiddleCenter);
	}
#endif
};

//具有绝对宽度的 的弹簧
class HSpacer :public Spacer {
private:
	HSpacer() {};
public:
	HSpacer(int fixedWidth) {
		SetFixedWidth(fixedWidth);
	}
#ifdef DEBUGPAINT
	void OnPaint(Painter& pt, PaintEventArgs& args) {
		pt.DrawString(std::to_string(GetFixedWidth()), GetFontFamily(this->State), 8, GetForeColor(this->State), RectF(0, 0, (float)_rect.Width, (float)_rect.Height), TextAlign::MiddleCenter);
	}
#endif
};

class ScrollBar :public Control {
public:
	ScrollBar() {
		this->_Type = ControlType::ControlScrollBar;
		Style.BackgroundColor = { 240,240,240 };//the bar backgroundcolor
		Style.ForeColor = { 155,155,155 };//the slider color
	}
	~ScrollBar() {}
	virtual void Move(double pos) {}//move silder use absolutely
	virtual Rect GetSliderRect() { return Rect(); }//
	virtual  int RollingCurrent() { return 0; }
	virtual int RollingTotal() { return 0; }//
	UIFunc<void(int, int)> Rolling = NULL;
};
