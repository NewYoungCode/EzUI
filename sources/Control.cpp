#include "Control.h"
namespace EzUI {
	inline bool __IsValid(int_t value) {
		return value != 0;
	}
	inline bool __IsValid(WORD value) {
		return value != 0;
	}
	inline bool __IsValid(BYTE value) {
		return value != 0;
	}
	inline bool __IsValid(float value) {
		return value != 0;
	}
	inline bool __IsValid(const Image* value) {
		return value != NULL;
	}
	inline bool __IsValid(const Color& value) {
		return value.GetValue() != 0;
	}
	inline bool __IsValid(const EString& value) {
		return !value.empty();
	}
	inline bool __IsValid(const std::wstring& value) {
		return !value.empty();
	}

#define UI_SUPER_STYLE_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
	if (_state == ControlState::None) {\
		_state = this->State;\
	}\
	##_type _##_filed;\
loop:\
	Control* pControl = this;\
	while (pControl)/* 如果没有则从父控件里面查找对应的样式 */{\
		_##_filed = pControl->GetStyle(_state). ##_filed;\
		if (__IsValid(_##_filed)) {\
			return _##_filed; /* 如果从父控件里面查找到就返回 */\
		}\
		pControl = pControl->Parent;\
	}\
	/* 如果没有active样式 则采用hover样式 */\
	if (!__IsValid(_##_filed) && _state == ControlState::Active) {\
		_state = ControlState::Hover;\
		goto loop;\
	}\
	if (!__IsValid(_##_filed) && _state != ControlState::Static) {\
		_state = ControlState::Static;/* 如果从父样式中仍然未找到,则找静态样式 */ \
		goto loop;\
	}\
	return _##_filed;\
	}\

#define UI_STYLE_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
/* 先根据对应的状态来获取样式  */ \
ControlStyle& stateStyle = this->GetStyle(this->State);\
if(__IsValid(stateStyle.##_filed)){\
	return stateStyle.##_filed; \
}\
/* 如果没有active样式 则采用hover样式 */\
if(this->State==ControlState::Active){\
	if(__IsValid(this->GetStyle(ControlState::Hover).##_filed)){\
	return this->GetStyle(ControlState::Hover).##_filed; \
	}\
}\
/* 获取不同的控件中默认样式  */ \
ControlStyle& defaultStyle = this->GetDefaultStyle(); \
if(__IsValid(defaultStyle.##_filed)){\
	return  defaultStyle.##_filed;\
\
}\
/* 以上两种样式都未获取成功的情况下才采用此样式 */ \
	return this->Style.##_filed;\
}\


#define UI_BORDER_BINDFUNC(_type,_filed1,_filed)  _type Control:: ##Get ##_filed1 ##_filed(ControlState _state)  { \
/* 先根据对应的状态来获取样式  */ \
ControlStyle& stateStyle = this->GetStyle(this->State);\
if(__IsValid(stateStyle .##_filed1. ##_filed)){\
	return stateStyle .##_filed1.##_filed; \
}\
/* 如果没有active样式 则采用hover样式 */\
if(this->State==ControlState::Active){\
	if(__IsValid(this->GetStyle(ControlState::Hover) .##_filed1.##_filed)){\
	return this->GetStyle(ControlState::Hover) .##_filed1.##_filed; \
	}\
}\
/* 获取不同的控件中默认样式  */ \
ControlStyle& defaultStyle = this->GetDefaultStyle(); \
if(__IsValid(defaultStyle .##_filed1.##_filed)){\
return  defaultStyle .##_filed1.##_filed;\
\
}\
/* 以上两种样式都未获取成功的情况下才采用此样式 */ \
	return this->Style .##_filed1.##_filed;\
}\
//end
};
namespace EzUI {

	UI_BORDER_BINDFUNC(int_t, Border, TopLeftRadius);
	UI_BORDER_BINDFUNC(int_t, Border, TopRightRadius);
	UI_BORDER_BINDFUNC(int_t, Border, BottomRightRadius);
	UI_BORDER_BINDFUNC(int_t, Border, BottomLeftRadius);
	UI_BORDER_BINDFUNC(int_t, Border, Left);
	UI_BORDER_BINDFUNC(int_t, Border, Top);
	UI_BORDER_BINDFUNC(int_t, Border, Right);
	UI_BORDER_BINDFUNC(int_t, Border, Bottom);
	UI_BORDER_BINDFUNC(Color, Border, Color);

	UI_STYLE_BINDFUNC(Color, BackColor);
	UI_STYLE_BINDFUNC(Image*, ForeImage);
	UI_STYLE_BINDFUNC(Image*, BackImage);
	UI_STYLE_BINDFUNC(float, Angle);

	UI_SUPER_STYLE_BINDFUNC(int_t, FontSize);
	UI_SUPER_STYLE_BINDFUNC(Color, ForeColor);
	UI_SUPER_STYLE_BINDFUNC(std::wstring, FontFamily);

	Control::Control() {}
	void Control::OnChildPaint(PaintEventArgs& args)
	{
		ViewControls.clear();
		//绘制子控件
		Rect rect(0, 0, Width(), Height());
		for (auto& it : _controls) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			it->SendNotify(args);
		}
	}
	void Control::OnPaint(PaintEventArgs& args)
	{
		OnBackgroundPaint(args);//先绘制背景
		OnForePaint(args);//再绘制前景
	}
	void Control::OnBackgroundPaint(PaintEventArgs& e)
	{
		const Color& backgroundColor = this->GetBackColor();
		Image* backgroundImage = this->GetBackImage();

		if (backgroundColor.GetValue() != 0) {
			e.Graphics.SetColor(backgroundColor);
			e.Graphics.FillRectangle(RectF{ 0,0,(float)_rect.Width,(float)_rect.Height });
		}
		if (backgroundImage != NULL) {
			e.Graphics.DrawImage(backgroundImage, RectF{ 0,0,(float)_rect.Width,(float)_rect.Height });
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		Image* foreImage = this->GetForeImage();
		if (foreImage) {
			e.Graphics.DrawImage(foreImage, RectF{ 0,0,(float)_rect.Width,(float)_rect.Height });
		}
	}
	void Control::OnBorderPaint(PaintEventArgs& e, const Border& border)
	{
		const Color& borderColor = border.Color;
		if (borderColor.GetValue() == 0) return;//边框无效颜色不绘制
		int_t borderLeft = border.Left;
		int_t borderTop = border.Top;
		int_t borderRight = border.Right;
		int_t borderBottom = border.Bottom;
		int_t topLeftRadius = border.TopLeftRadius;
		int_t topRightRadius = border.TopRightRadius;
		int_t bottomRightRadius = border.BottomRightRadius;
		int_t bottomLeftRadius = border.BottomLeftRadius;

		//决定绘制虚线还是实线
		e.Graphics.SetStrokeStyle(border.BorderStyle);
		//规则的矩形
		if (topLeftRadius == 0 && topRightRadius == 0 && bottomLeftRadius == 0 && bottomRightRadius == 0) {
			bool hasBorder = borderLeft || borderTop || borderRight || borderBottom;
			if (!hasBorder) return;//边框为0不绘制
			e.Graphics.SetColor(borderColor);
			if (borderLeft > 0) {
				RectF rect(0, 0, (float)borderLeft, (float)Height());
				e.Graphics.FillRectangle(rect);
			}
			if (borderTop > 0) {
				RectF rect(0, 0, (float)Width(), (float)borderTop);
				e.Graphics.FillRectangle(rect);
			}
			if (borderRight > 0) {
				RectF rect((float)(_rect.Width - borderRight), 0, (float)borderRight, (float)Height());
				e.Graphics.FillRectangle(rect);
			}
			if (borderBottom > 0) {
				RectF rect(0, (float)(_rect.Height - borderBottom), (float)Width(), (float)borderBottom);
				e.Graphics.FillRectangle(rect);
			}
		}
		else {
			int_t value1 = borderLeft > borderTop ? borderLeft : borderTop;
			int_t value2 = borderRight > borderBottom ? borderRight : borderBottom;
			int_t maxBorder = value1 > value2 ? value1 : value2;
			e.Graphics.SetColor(borderColor);
			Geometry rr(Rect(0, 0, Width(), Height()), topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
			e.Graphics.DrawGeometry(rr.Get(), maxBorder);
		}
	}
	ControlStyle& Control::GetStyle(const ControlState& _state) {
		if (_state == ControlState::Static) {
			return this->Style;
		}
		if (_state == ControlState::Hover) {
			return this->HoverStyle;
		}
		if (_state == ControlState::Active) {
			return this->ActiveStyle;
		}
		return this->Style;
	}
	ControlStyle& Control::GetDefaultStyle() {
		return this->Style;
	}

	void Control::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		__super::SetAttribute(attrName, attrValue);
		do
		{
			if (attrName == "name" || attrName == "id") {
				this->Name = attrValue;
				break;
			}
			if (attrName == "point" || attrName == "location") {
				auto arr = attrValue.split(",");
				int_t x = std::stoi(arr[0]);
				int_t y = std::stoi(arr[1]);
				this->SetLocation(Point(x, y));
				break;
			}
			if (attrName == "size") {
				auto arr = attrValue.split(",");
				int_t width = std::stoi(arr[0]);
				int_t height = std::stoi(arr[1]);
				this->SetFixedSize(Size(width, height));
				break;
			}
			if (attrName == "rect" && !attrValue.empty()) {
				auto rectStr = attrValue.split(",");
				Rect rect;
				rect.X = std::stoi(rectStr.at(0));
				rect.Y = std::stoi(rectStr.at(1));
				rect.Width = std::stoi(rectStr.at(2));
				rect.Height = std::stoi(rectStr.at(3));
				this->SetRect(rect);
				break;
			}
			if (attrName == "margin-left") {
				this->Margin.Left = std::stoi(attrValue);
				break;
			}
			if (attrName == "margin-top") {
				this->Margin.Top = std::stoi(attrValue);
				break;
			}
			if (attrName == "margin-right") {
				this->Margin.Right = std::stoi(attrValue);
				break;
			}
			if (attrName == "margin-bottom") {
				this->Margin.Bottom = std::stoi(attrValue);
				break;
			}
			if (attrName == "margin") {//遵循web前端的规则
				auto strs = attrValue.split(",");
				if (strs.size() == 1) {
					this->Margin = std::stoi(strs[0]);
					break;
				}
				if (strs.size() == 2) {
					this->Margin.Top = this->Margin.Bottom = std::stoi(strs[0]);
					this->Margin.Left = this->Margin.Right = std::stoi(strs[1]);
					break;
				}
				if (strs.size() == 4) {
					this->Margin.Top = std::stoi(strs[0]);
					this->Margin.Right = std::stoi(strs[1]);
					this->Margin.Bottom = std::stoi(strs[2]);
					this->Margin.Left = std::stoi(strs[3]);
					break;
				}
				break;
			}
			if (attrName == "x") {
				this->SetLocation({ std::stoi(attrValue) ,this->GetRect().Y });
				break;
			}
			if (attrName == "y") {
				this->SetLocation({ this->GetRect().X, std::stoi(attrValue) });
				break;
			}
			if (attrName == "autosize" && attrValue == "true") {
				this->SetAutoWidth(true);
				this->SetAutoHeight(true);
				break;
			}
			if (attrName == "width") {
				if (attrValue == "auto") {
					this->SetAutoWidth(true);
				}
				else {
					//如果单独设置了宽高那就是绝对宽高了
					this->SetFixedWidth(std::stoi(attrValue));
				}
				break;
			}
			if (attrName == "height") {
				if (attrValue == "auto") {
					this->SetAutoHeight(true);
				}
				else {
					//如果单独设置了宽高那就是绝对宽高了
					this->SetFixedHeight(std::stoi(attrValue));
				}
				break;
			}
			if (attrName == "visible" || attrName == "display") {
				this->_visible = (::strcmp(attrValue.c_str(), "true") == 0 ? true : false);
				break;
			}
			if (attrName == "tips") {
				this->SetTips(attrValue);
				break;
			}
			if (attrName == "dock") {
				if (attrValue == "fill") {
					this->_dock = DockStyle::Fill; break;
				}
				if (attrValue == "horizontal") {
					this->_dock = DockStyle::Horizontal; break;
				}
				if (attrValue == "vertical") {
					this->_dock = DockStyle::Vertical; break;
				}
				break;
			}
			if (attrName == "action") {
				if (attrValue == "close") {
					this->Action = ControlAction::Close; break;
				}
				if (attrValue == "mini") {
					this->Action = ControlAction::Mini; break;
				}
				if (attrValue == "max") {
					this->Action = ControlAction::Max; break;
				}
				if (attrValue == "title") {
					this->Action = ControlAction::Title; break;
				}
				if (attrValue == "move" || attrValue == "movewindow") {
					this->Action = ControlAction::MoveWindow; break;
				}
				break;
			}
			if (attrName == "enable") {
				if (attrValue == "true") {
					this->Enable = true;
					break;
				}
				if (attrValue == "false") {
					this->Enable = false;
					break;
				}
			}
			if (attrName == "disabled") {
				this->Enable = false;
				break;
			}
			if (attrName == "scrollbar") {
				ScrollBar* sb = this->GetScrollBar();
				if (sb) {
					sb->SetAttribute("name", attrValue);
				}
				break;
			}
		} while (false);
	}
	const Controls& Control::GetViewControls()
	{
		return this->ViewControls;
	}
	float Control::GetScale()
	{
		return this->_scale;
	}
	HCURSOR Control::GetHCursor() {
		if (this->HoverStyle.Cursor != NULL) {
			return this->HoverStyle.Cursor;
		}
		else if (this->Style.Cursor != NULL) {
			return this->Style.Cursor;
		}
		return NULL;
	}
	int_t Control::X()
	{
		return _rect.X;
	}
	int_t Control::Y()
	{
		return _rect.Y;
	}
	int_t Control::Width()
	{
		return _rect.Width;
	}
	int_t Control::Height()
	{
		return _rect.Height;
	}
	void Control::SetX(int_t X) {
		SetLocation({ X,_rect.Y });
	}
	void Control::SetY(int_t Y) {
		SetLocation({ _rect.X,Y });
	}
	void Control::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, _rect.Width, _rect.Height));
	}
	void Control::SetWidth(int_t width) {
		SetSize({ width,_rect.Height });
	}
	void Control::SetHeight(int_t height) {
		SetSize({ _rect.Width,height });
	}
	void Control::SetSize(const Size& size)
	{
		SetRect({ _rect.X,_rect.Y,size.Width,size.Height });
	}
	Point Control::GetLocation()
	{
		return _rect.GetLocation();
	}
	Size Control::GetSize()
	{
		return _rect.GetSize();
	}
	const Rect& Control::GetRect()
	{
		return _rect;
	}
	void Control::SetFixedWidth(int_t fixedWidth)
	{
		_fixedSize.Width = fixedWidth;
		SetRect({ _rect.X,_rect.Y,fixedWidth,_rect.Height });
	}
	void Control::SetFixedHeight(int_t fixedHeight)
	{
		_fixedSize.Height = fixedHeight;
		SetRect({ _rect.X,_rect.Y,_rect.Width,fixedHeight });
	}
	void Control::SetFixedSize(const Size& size)
	{
		_fixedSize.Width = size.Width;
		_fixedSize.Height = size.Height;
		SetRect({ _rect.X,_rect.Y,size.Width,size.Height });
	}
	int_t Control::GetFixedWidth()
	{
		return _fixedSize.Width;
	}
	int_t Control::GetFixedHeight()
	{
		return _fixedSize.Height;
	}
	bool Control::IsPendLayout() {
		return this->_layoutState == LayoutState::Pend;
	}
	const LayoutState Control::TryPendLayout() {
		if (this->_layoutState == LayoutState::None) {
			this->_layoutState = LayoutState::Pend;
		}
		return this->_layoutState;
	}
	const LayoutState Control::GetLayoutState()
	{
		return this->_layoutState;
	}
	void Control::EndLayout() {
		this->_layoutState = LayoutState::None;
	}
	Rect Control::GetClientRect() {
		Control* pCtrl = this;
		const Rect& rect = GetRect();
		int_t x = rect.X;
		int_t y = rect.Y;
		while ((pCtrl = pCtrl->Parent))
		{
			x += pCtrl->X();
			y += pCtrl->Y();
		}
		return Rect{ x,y,Width(),Height() };
	}
	const Rect& Control::GetViewRect()
	{
		return this->_viewRect;
	}
	DockStyle Control::GetDockStyle()
	{
		return this->_dock;
	}
	void Control::SetDockStyle(const DockStyle& dockStyle)
	{
		if (dockStyle != this->_dock && Parent) {
			Parent->TryPendLayout();
		}
		this->_dock = dockStyle;
	}
	const Rect& Control::SetRect(const Rect& rect)
	{
		this->_rect = rect;

		if (GetFixedWidth() > 0) {
			_rect.Width = GetFixedWidth();
		}
		if (GetFixedHeight() > 0) {
			_rect.Height = GetFixedHeight();
		}

		Point newLocation = _rect.GetLocation();
		Size newSize = _rect.GetSize();

		if (!_lastLocation.Equals(newLocation)) {
			this->SendNotify(MoveEventArgs(newLocation));
			_lastLocation = newLocation;
		}
		if (!newSize.Equals(_lastSize)) {
			this->SendNotify(SizeEventArgs(newSize));
			_lastSize = newSize;
		}
		return this->_rect;
	}
	void Control::SetTips(const EString& text)
	{
		_tipsText = text;
	}
	const EString& Control::GetTips()
	{
		return _tipsText;
	}
	void Control::RefreshLayout()
	{
		if (this->_layoutState == LayoutState::Layouting) {
			return;
		}
		this->_layoutState = LayoutState::Layouting;//布局中
		if (GetScrollBar()) {//如果存在滚动条就设置滚动条的矩形位置
			GetScrollBar()->ParentSize({ _rect.Width,_rect.Height });
		}
		_contentSize.Width = 0;
		_contentSize.Height = 0;
		this->OnLayout();
		if (IsAutoHeight() && Height() != _contentSize.Height) {
			this->SetFixedHeight(_contentSize.Height);
			this->EndLayout();
			this->RefreshLayout();
			if (Parent) {
				Parent->Invalidate();
			}
			return;
		}
		if (IsAutoWidth() && Width() != _contentSize.Width) {
			this->SetFixedWidth(_contentSize.Width);
			this->EndLayout();
			this->RefreshLayout();
			if (Parent) {
				Parent->Invalidate();
			}
			return;
		}
		this->EndLayout();
	}
	void Control::OnLayout() {
		_contentSize.Width = 0;
		_contentSize.Height = 0;
		int_t _width;
		int_t _height;
		for (auto& it : GetControls()) {

			if (it->GetDockStyle() == DockStyle::Fill) {
				it->SetRect(Rect{ 0,0,Width(),Height() });
			}
			else if (it->GetDockStyle() == DockStyle::Vertical) {
				it->SetRect(Rect{ 0,0,it->Width(),Height() });
			}
			else if (it->GetDockStyle() == DockStyle::Horizontal) {
				it->SetRect(Rect{ 0,0,Width(),it->Height() });
			}

			_width = it->X() + it->Width();
			if (_width > _contentSize.Width) {
				_contentSize.Width = _width;
			}
			_height = it->Y() + it->Height();
			if (_height > _contentSize.Height) {
				_contentSize.Height = _height;
			}
		}
		this->EndLayout();
	}
	bool Control::SendNotify(const EventArgs& arg)
	{
		return this->OnEvent((EventArgs&)arg);
	}
	bool Control::OnEvent(EventArgs& arg)
	{
		bool isRemove = false;
		this->_isRemove = &isRemove;
		do
		{
			if (arg.EventType == Event::OnPaint && !IsVisible()) {
				break;
			}
			if (PublicData && ((this->EventNotify & arg.EventType) == arg.EventType)) {
				if (arg.EventType != Event::OnPaint) {
					bool bHandle = false;
					PublicData->SendNotify(this, (EventArgs&)arg, bHandle);
					if (bHandle) {
						//如果处理过了则不需要继续往下派发
						break;
					}
					if (isRemove) {
						break;
					}
				}
			}
			if (arg.EventType == Event::OnPaint) {
				this->OnPaintBefore((PaintEventArgs&)arg);
				break;
			}
			if (arg.EventType == Event::OnMove) {
				this->OnMove((MoveEventArgs&)arg);
				break;
			}
			if (arg.EventType == Event::OnSize) {
				this->OnSize((SizeEventArgs&)arg);
				break;
			}
			if (arg.EventType == Event::OnDpiChange) {
				this->OnDpiChange((DpiChangeEventArgs&)arg);
				break;
			}
			if ((arg.EventType & Event::OnMouseEvent) == arg.EventType) {
				this->OnMouseEvent((MouseEventArgs&)arg);
				break;
			}
			if ((arg.EventType & Event::OnKeyBoardEvent) == arg.EventType) {
				this->OnKeyBoardEvent((KeyboardEventArgs&)arg);
				break;
			}
			if (arg.EventType == Event::OnFocus) {
				this->OnFocus((FocusEventArgs&)arg);
				break;
			}
			if (arg.EventType == Event::OnKillFocus) {
				this->OnKillFocus((KillFocusEventArgs&)arg);
				break;
			}
		} while (false);
		if (!isRemove) {
			//通用事件处理 ps:绘制函数比较特殊(在其他地方处理)
			if (this->Notify && (arg.EventType != Event::OnPaint)) {
				bool bHandle = false;
				this->Notify(this, arg, bHandle);
			}
			if (!isRemove) {
				this->_isRemove = NULL;
				return true;
			};
		}
		return false;
	}

	//专门处理键盘消息的
	void Control::OnKeyBoardEvent(const KeyboardEventArgs& _args) {
		do
		{
			if (Enable == false) break;
			KeyboardEventArgs& args = (KeyboardEventArgs&)_args;
			if ((this->EventPassThrough & args.EventType) == args.EventType && this->Parent) {//检查鼠标穿透
				KeyboardEventArgs copy_args = args;
				this->Parent->SendNotify(copy_args);//如果设置了穿透就直接发送给上一层控件
			}
			switch (args.EventType)
			{
			case Event::OnKeyChar: {
				OnKeyChar(args);
				break;
			}
			case Event::OnKeyDown: {
				OnKeyDown(args);
				break;
			}
			case Event::OnKeyUp: {
				OnKeyUp(args);
				break;
			}
			default:
				break;
			}
		} while (false);
	}
	//专门处理鼠标消息的
	void Control::OnMouseEvent(const MouseEventArgs& _args) {
		if (!Enable) return;
		MouseEventArgs& args = (MouseEventArgs&)_args;
		do
		{
			switch (args.EventType)
			{
			case Event::OnMouseWheel: {
				OnMouseWheel(args);
				break;
			}
			case Event::OnMouseClick: {
				OnMouseClick(args);
				break;
			}
			case Event::OnMouseEnter: {
				OnMouseEnter(args);
				break;
			}
			case Event::OnMouseMove: {
				OnMouseMove(args);
				break;
			}
			case Event::OnMouseDoubleClick: {
				OnMouseDoubleClick(args);
				break;
			}
			case Event::OnMouseDown: {
				OnMouseDown(args);
				break;
			}
			case Event::OnMouseUp: {
				OnMouseUp(args);
				break;
			}
			case Event::OnMouseLeave: {
				OnMouseLeave(args);
				break;
			}
			default:
				break;
			}
		} while (false);
		if ((this->EventPassThrough & args.EventType) == args.EventType && this->Parent) {//检查鼠标穿透
			MouseEventArgs copy_args = args;
			copy_args.Location.X += this->X();
			copy_args.Location.Y += this->Y();
			this->Parent->SendNotify(copy_args);//如果设置了穿透就发送给上一层控件
		}
	}
	void Control::OnPaintBefore(PaintEventArgs& args) {
		this->PublicData = args.PublicData;
		if (this->IsPendLayout()) {//绘制的时候会检查时候有挂起的布局 如果有 立即让布局生效并重置布局标志
			this->RefreshLayout();
		}
		auto clientRect = this->GetClientRect();//获取基于窗口的位置
		if (clientRect.IsEmptyArea()) { return; }
		auto& invalidRect = args.InvalidRectangle;
		auto& pt = args.Graphics;
		Rect _ClipRect = clientRect;
		this->ComputeClipRect();//重新计算基于父亲的裁剪区域
		if (!Rect::Intersect(_ClipRect, this->_viewRect, invalidRect)) {//和重绘区域进行裁剪
			return;
		}

		//绘制数量+1
		++args.PublicData->PaintCount;
		//设置绘制偏移 以及旋转
		args.PushOffset({ clientRect.X ,clientRect.Y });
		float angle = this->GetAngle();
		if (angle != 0) {//围绕着控件中心点旋转
			float pointX = clientRect.X + clientRect.Width / 2.0f;
			float pointY = clientRect.Y + clientRect.Height / 2.0f;
			pt.SetTransform(pointX, pointY, angle);
		}
		//border信息
		Border border;
		border.Left = GetBorderLeft();
		border.Top = GetBorderTop();
		border.Right = GetBorderRight();
		border.Bottom = GetBorderBottom();
		border.TopLeftRadius = GetBorderTopLeftRadius();
		border.TopRightRadius = GetBorderTopRightRadius();
		border.BottomRightRadius = GetBorderBottomRightRadius();
		border.BottomLeftRadius = GetBorderBottomLeftRadius();
		bool hasRadius = border.TopLeftRadius || border.TopRightRadius || border.BottomRightRadius || border.BottomLeftRadius;
#if USED_DIRECT2D
		if (hasRadius) {
			//处理圆角控件 使用纹理的方式 (这样做是为了控件内部无论怎么绘制都不会超出圆角部分) 带抗锯齿
			Geometry roundRect(Rect(0, 0, clientRect.Width, clientRect.Height), border.TopLeftRadius, border.TopRightRadius, border.BottomRightRadius, border.BottomLeftRadius);
			Geometry _clientRect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height);
			Geometry outClipRect;
			Geometry::Intersect(outClipRect, roundRect, _clientRect);
			args.PushLayer(outClipRect);
		}
		else {
			//针对矩形控件
			args.PushLayer(Rect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height));
		}
#endif 
		//调用公共函数,如果那边不拦截,就开始绘制自身基本上下文
		bool bHandle = false;
		PublicData->SendNotify(this, args, bHandle);
		if (!bHandle) {
			this->OnPaint(args);
		}

		//绘制子控件
		this->OnChildPaint(args);
		//绘制滚动条
		EzUI::ScrollBar* scrollbar = NULL;
		if (scrollbar = this->GetScrollBar()) {
			scrollbar->PublicData = args.PublicData;
			scrollbar->SendNotify(args);
		}
		//绘制边框
		border.Color = GetBorderColor();
		this->OnBorderPaint(args, border);//绘制边框
#ifdef _DEBUG
		if (PublicData->Debug) {
			float width = 1 * this->GetScale();
			pt.SetColor(PublicData->DebugColor);
			pt.DrawRectangle(RectF(0, 0, clientRect.Width, clientRect.Height), 0, width);
		}
#endif
		if (this->Notify) {
			bool bHandle = false;
			this->Notify(this, args, bHandle);
		}
		args.PopLayer();//弹出纹理层
		args.PopOffset();//弹出偏移
		if (angle != 0) {
			pt.SetTransform(0, 0, 0);
		}
	}
	void Control::OnDpiChange(const DpiChangeEventArgs& arg)
	{
		float scale = arg.Scale / this->_scale;
		this->_scale = arg.Scale;

		bool needScale = false;
		if (scale != 1.0f) {
			needScale = true;
			int_t fw = this->GetFixedWidth() * scale + 0.5;
			int_t fh = this->GetFixedHeight() * scale + 0.5;

			if (fw && fh) {
				this->SetFixedSize(Size(fw, fh));
			}
			else if (fw) {
				this->SetFixedWidth(fw);
			}
			else if (fh) {
				this->SetFixedHeight(fh);
			}

			Rect newRect = GetRect();
			this->SetRect(newRect.Scale(scale));

			this->Margin.Scale(scale);
			this->Style.Scale(scale);
			this->ActiveStyle.Scale(scale);
			this->HoverStyle.Scale(scale);
		}

		for (auto& it : GetControls()) {
			it->SendNotify(arg);
		}

		if (needScale && this->GetScrollBar()) {
			this->GetScrollBar()->SendNotify(arg);
		}

	}
	Control::~Control()
	{
		if (PublicData) {
			PublicData->RemoveControl(this);
			PublicData = NULL;
		}
		DestroySpacers();
		Parent = NULL;
	}
	void Control::DestroySpacers() {
		//控件释放的时候自动释放弹簧
		auto spacersCopy = _spacers;  // 复制一份避免迭代器错误
		for (auto& it : spacersCopy) {
			this->Remove(it, true);//移除且释放控件
		}
		_spacers.clear(); // 如果需要的话，清空原列表
	}
	int_t Control::IndexOf(Control* childCtl)
	{
		const auto& pControls = this->GetControls();
		int_t pos = 0;
		for (auto itor = pControls.begin(); itor != pControls.end(); ++itor)
		{
			if (dynamic_cast<Spacer*>(*itor)) {
				continue;
			}
			if ((*itor) == childCtl) {
				return pos;
			}
			++pos;
		}
		return size_t(-1);
	}
	Control* Control::Add(Control* ctl) {
#ifdef _DEBUG
		auto itor = std::find(_controls.begin(), _controls.end(), ctl);
		if (itor != _controls.end()) {
			ASSERT(!"The control already exists and cannot be added repeatedly");
		}
#endif
		if (dynamic_cast<Spacer*>(ctl)) {
			_spacers.push_back(ctl);
		}
		_controls.push_back(ctl);
		ctl->PublicData = this->PublicData;
		ctl->Parent = this;

		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendNotify(DpiChangeEventArgs(this->GetScale()));
		}

		ctl->TryPendLayout();
		this->TryPendLayout();//添加控件需要将布局重新挂起
		return ctl;
	}
	void Control::Insert(int_t pos, Control* ctl)
	{
#ifdef _DEBUG
		{
			auto itor = std::find(_controls.begin(), _controls.end(), ctl);
			if (itor != _controls.end()) {
				ASSERT(!"The control already exists and cannot be added repeatedly");
			}
		}
#endif
		if (dynamic_cast<Spacer*>(ctl)) {
			_spacers.push_back(ctl);
		}
		size_t i = 0;
		auto itor = _controls.begin();
		for (; itor != _controls.end(); ++itor) {
			if (i == pos) {
				break;
			}
			++i;
		}
		if (itor == _controls.end()) {
			_controls.push_back(ctl);
		}
		else {
			_controls.insert(itor, ctl);
		}
		ctl->PublicData = this->PublicData;
		ctl->Parent = this;
		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendNotify(DpiChangeEventArgs(this->GetScale()));
		}
		ctl->TryPendLayout();
		this->TryPendLayout();//添加控件需要将布局重新挂起
	}
	void Control::SetParent(Control* parentCtl)
	{
		parentCtl->Add(this);
	}

	void Control::Remove(Control* ctl, bool freeCtrl)
	{
		//寻找控件看是否包含
		auto itor = ::std::find(_controls.begin(), _controls.end(), ctl);
		if (itor != _controls.end()) {
			ctl->OnRemove();
			this->TryPendLayout();//移除控件需要将布局重新挂起
			_controls.erase(itor);
			auto itor2 = ::std::find(ViewControls.begin(), ViewControls.end(), ctl);
			if (itor2 != ViewControls.end()) {
				ViewControls.erase(itor2);
			}
			if (freeCtrl) {
				delete ctl;
			}
		}
		//如果是弹簧控件 顺便把弹簧容器中的item也移除
		auto itorSpacer = ::std::find(_spacers.begin(), _spacers.end(), ctl);
		if (itorSpacer != _spacers.end()) {
			_spacers.erase(itorSpacer);
		}
	}
	void Control::OnRemove()
	{
		if (_isRemove) {
			*_isRemove = true;
		}
		for (auto& it : _controls) {
			it->OnRemove();
		}
		if (PublicData) {
			PublicData->RemoveControl(this);
			PublicData = NULL;
		}
	}
	Control* Control::FindControl(const EString& ctlName)
	{
		if (ctlName.empty()) {
			return NULL;
		}
		if (this->Name == ctlName) {
			return this;
		}
		for (auto& it : (this->_controls))
		{
			if (it->Name == ctlName) {
				return it;
			}
			auto ctl = it->FindControl(ctlName);
			if (ctl) return ctl;
		}
		return NULL;
	}
	Controls Control::FindControl(const EString& attrName, const EString& attrValue)
	{
		Controls ctls;
		if (attrName.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->_controls))
		{
			if (it->GetAttribute(attrName) == attrValue) {
				ctls.push_back(it);
			}
			auto _ctls = it->FindControl(attrName, attrValue);
			for (auto& it2 : _ctls) {
				ctls.push_back(it2);
			}
		}
		return ctls;
	}
	Control* Control::FindSingleControl(const EString& attrName, const EString& attrValue) {
		auto list = this->FindControl(attrName, attrValue);
		if (list.size() > 0) {
			return *list.begin();
		}
		return NULL;
	}

	Control* Control::FindChild(const EString& ctlName)
	{
		if (ctlName.empty()) {
			return NULL;
		}
		for (auto& it : (this->_controls))
		{
			if (it->Name == ctlName) {
				return it;
			}
		}
		return NULL;
	}
	Controls Control::FindChild(const EString& attrName, const EString& attrValue)
	{
		Controls ctls;
		if (attrName.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->_controls))
		{
			if (it->GetAttribute(attrName) == attrValue) {
				ctls.push_back(it);
			}
		}
		return ctls;
	}
	Control* Control::FindSingleChild(const EString& ctlName, const EString& attrValue) {
		auto list = this->FindChild(ctlName, attrValue);
		if (list.size() > 0) {
			return *list.begin();
		}
		return NULL;
	}

	bool Control::SwapChild(Control* ct1, Control* ct2)
	{
		int_t swapCount = 0;
		for (auto& it : this->_controls) {
			if (swapCount == 2) {
				break;
			}
			if (it == ct1) {
				it = ct2;
				++swapCount;
				continue;
			}
			if (it == ct2) {
				it = ct1;
				++swapCount;
				continue;
			}
		}
		if (swapCount == 2) {
			this->TryPendLayout();
			return true;
		}
		return false;
	}
	void Control::SetVisible(bool flag) {
		if (flag != this->_visible && this->Parent) {
			this->Parent->TryPendLayout();
		}
		this->_visible = flag;
	}
	bool Control::IsVisible() {
		return this->_visible;
	}

	//void CalculateRotatedMaxRect(const Rect& rect, double angle, RectF* outRect) {
	//	// 将角度转换为弧度
	//	double angleRad = angle * 3.14159265358979323846 / 180.0;
	//	// 计算旋转后的矩形的宽度和高度
	//	double newWidth = fabs(rect.Width * cos(angleRad)) + fabs(rect.Height * sin(angleRad));
	//	double newHeight = fabs(rect.Width * sin(angleRad)) + fabs(rect.Height * cos(angleRad));
	//	// 计算旋转后的矩形的中心点坐标
	//	double centerX = rect.X + rect.Width / 2.0f;
	//	double centerY = rect.Y + rect.Height / 2.0f;
	//	// 计算新矩形的位置
	//	outRect->X = centerX - newWidth / 2.0f;
	//	outRect->Y = centerY - newHeight / 2.0f;
	//	outRect->Width = newWidth;
	//	outRect->Height = newHeight;
	//}
	/* 	RectF rect;
	CalculateRotatedMaxRect(_InvalidateRect, angle, &rect);
	Rect r2(rect.X, rect.Y, rect.Width, rect.Height);
	winData->InvalidateRect(&r2); */

	bool Control::Invalidate() {
		if (PublicData) {
			if (Parent && this->IsPendLayout()) {
				return Parent->Invalidate();
			}
			float angle = this->GetAngle();
			if (!(angle == 0 || angle == 180) && Parent) {
				return Parent->Invalidate();
			}
			WindowData* winData = PublicData;
			if (winData) {
				Rect _InvalidateRect = GetClientRect();
				winData->InvalidateRect(_InvalidateRect);
				return true;
			}
		}
		return false;
	}
	void Control::Refresh() {
		if (Invalidate()) {
			PublicData->UpdateWindow();//立即更新全部无效区域
		}
	}
	Rect Control::GetCareRect()
	{
		return Rect();
	}
	const Size& Control::GetContentSize()
	{
		return _contentSize;
	}
	bool Control::IsAutoWidth()
	{
		return this->_autoWidth;
	}
	void Control::SetAutoSize(bool flag)
	{
		this->SetAutoWidth(flag);
		this->SetAutoHeight(flag);
	}
	void Control::SetAutoWidth(bool flag)
	{
		if (flag != this->_autoWidth && Parent) {
			Parent->TryPendLayout();
		}
		if (flag && Width() == 0) {
			_rect.Width = 1;
		}
		this->_autoWidth = flag;
	}
	bool Control::IsAutoHeight()
	{
		return this->_autoHeight;
	}
	void Control::SetAutoHeight(bool flag)
	{
		if (flag != this->_autoHeight && Parent) {
			Parent->TryPendLayout();
		}
		if (flag && Height() == 0) {
			_rect.Height = 1;
		}
		this->_autoHeight = flag;
	}
	void Control::SetContentWidth(int_t width)
	{
		this->_contentSize.Width = width;
	}
	void Control::SetContentHeight(int_t height)
	{
		this->_contentSize.Height = height;
	}
	void Control::SetContentSize(const Size& size)
	{
		this->_contentSize = size;
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->_viewRect);//引用父控件的裁剪区域
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->_viewRect);//自身和父控件对比较裁剪区域
		}
		else {
			Rect& ClipRectRef = *(Rect*)(&this->_viewRect);
			ClipRectRef = this->GetClientRect();
		}
	}
	const Controls& Control::GetControls()
	{
		return _controls;
	}
	Control* Control::GetControl(int_t pos)
	{
		int_t _pos = 0;
		for (auto& it : _controls) {
			if (_pos == pos) {
				return it;
			}
			++_pos;
		}
		return NULL;
	}
	bool Control::Contains(Control* ctl) {
		ASSERT(ctl);
		Control* pControl = ctl;
		while (pControl)
		{
			if (pControl == this || pControl == this->GetScrollBar()) {
				return true;
			}
			pControl = pControl->Parent;
		}
		return false;
	}

	void Control::Clear()
	{
		this->Clear(false);
	}

	void Control::Clear(bool freeChilds)
	{
		auto temp = _controls;
		for (auto itor = temp.begin(); itor != temp.end(); ++itor)
		{
			Control* it = *itor;
			it->OnRemove();
			if (freeChilds) {
				delete it;
			}
		}
		this->ViewControls.clear();//清空可见控件
		this->_controls.clear();//清空子控件集合
		this->_spacers.clear();//清空弹簧
		this->TryPendLayout();//挂起布局
		ScrollBar* scrollBar = this->GetScrollBar();
		if (scrollBar) {
			scrollBar->Reset();
		}
	}
	void Control::OnMouseMove(const MouseEventArgs& args)
	{
	}
	void Control::OnMouseWheel(const MouseEventArgs& args)
	{
	}
	void Control::OnMouseClick(const MouseEventArgs& args)
	{
	}
	void Control::OnMouseDoubleClick(const MouseEventArgs& args)
	{
	}
	void Control::OnMouseEnter(const MouseEventArgs& args)
	{
		this->State = ControlState::Hover;
		if (PublicData) {
			PublicData->SetTips(this, this->GetTips().unicode());
		}
		if (!(this->EventPassThrough & Event::OnMouseEnter)) {
			this->Invalidate();
		}
	}
	void Control::OnMouseDown(const MouseEventArgs& args)
	{
		this->State = ControlState::Active;
		if (ActiveStyle.Cursor) {
			::SetCursor(ActiveStyle.Cursor);
		}
		if (!(this->EventPassThrough & Event::OnMouseDown)) {
			this->Invalidate();
		}
	}
	void Control::OnMouseUp(const MouseEventArgs& args)
	{
		if (!(this->EventPassThrough & Event::OnMouseUp)) {
			this->Invalidate();
		}
	}
	void Control::OnMouseLeave(const MouseEventArgs& args)
	{
		this->State = ControlState::Static;
		if (!(this->EventPassThrough & Event::OnMouseLeave)) {
			this->Invalidate();
		}

	}
	void Control::OnKeyChar(const KeyboardEventArgs& args) {
	}
	void Control::OnKeyDown(const KeyboardEventArgs& args) {
	}
	void Control::OnKeyUp(const KeyboardEventArgs& args) {
	}
	void Control::OnFocus(const FocusEventArgs& _args)
	{
	}
	void Control::OnKillFocus(const KillFocusEventArgs& arg)
	{
	}
	void Control::OnMove(const MoveEventArgs& arg)
	{
	}
	void Control::OnSize(const SizeEventArgs& arg)
	{
		this->TryPendLayout();//将自己挂起
		if (Parent) {
			Parent->TryPendLayout();//将父控件挂起
		}
	}
	ScrollBar* Control::GetScrollBar()
	{
		return NULL;
	}
};

//滚动条相关
namespace EzUI {
	void ScrollBar::OnMouseUp(const MouseEventArgs& arg)
	{
		__super::OnMouseUp(arg);
		_mouseDown = false;
	}
	void  ScrollBar::OnMouseLeave(const MouseEventArgs& arg)
	{
		__super::OnMouseLeave(arg);
		_mouseDown = false;
	}
	ScrollBar::ScrollBar() {
		Style.ForeColor = Color(205, 205, 205);//the bar default backgroundcolor
		SetSize({ 10,10 });
	}
	ScrollBar::~ScrollBar() {}
	bool ScrollBar::IsDraw()
	{
		if (!Scrollable()) {
			return false;
		}
		return this->IsVisible();
	}
	void ScrollBar::Reset()
	{
		this->_sliderPos = 0;
		this->_offset = 0;
	}
	bool ScrollBar::Scrollable()
	{
		if (this->_overflowLength > 0) {
			return true;
		}
		return false;
	}
	void ScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (!Scrollable()) {
			return;
		}
		__super::OnBackgroundPaint(e);
	}
	void ScrollBar::OnForePaint(PaintEventArgs& args)
	{
		if (!Scrollable()) {
			return;
		}
		__super::OnForePaint(args);
		//绘制滑块
		RectF sliderRect = GetSliderRect();
		const Color& color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, GetBorderTopLeftRadius());
		}
	}

	void ScrollBar::ScrollTo(float scrollRate) {
		if (Parent == NULL) return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t offset = scrollRate * this->_overflowLength;
		ScrollTo(-offset, Event::None);
	}

	float ScrollBar::ScrollPos()
	{
		if (this->_overflowLength <= 0)  return 1.0f;
		return std::abs(this->_offset) * 1.0f / this->_overflowLength;
	}

	void ScrollBar::ScrollTo(int_t offset, const Event& type) {
		if (Parent == NULL) return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		//if (!Scrollable()) {
		//	return;
		//}
		int_t viewLength;
		int_t contentLength;
		int_t scrollBarLength;
		this->GetInfo(&viewLength, &contentLength, &scrollBarLength);
		if (offset > 0) {
			//滚动条在顶部
			this->_offset = 0;
			this->_sliderPos = 0;
		}
		else if (std::abs(offset) >= this->_overflowLength) {
			//滚动条在底部
			this->_offset = -this->_overflowLength;
			this->_sliderPos = scrollBarLength - this->_sliderLength;
		}
		else {
			//正常滚动
			this->_offset = offset;
			this->_sliderPos = -offset / this->_rollRate;
		}
		//调用容器的滚动函数进行偏移
		if (OffsetCallback) {
			OffsetCallback(this->_offset);
			SyncInfo();
		}
		Parent->Invalidate();
		//Parent->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
		if (Scroll) {
			Scroll(this, (double)this->_offset / (-this->_overflowLength), type);
		}
	}
	void ScrollBar::SyncInfo()
	{
		if (Parent == NULL)return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t scrollBarLength;
		this->GetInfo(&this->_viewLength, &this->_contentLength, &scrollBarLength);
		this->_overflowLength = this->_contentLength - this->_viewLength;//超出容器的内容长度
		if (_overflowLength > 0) {
			this->_sliderLength = (double)this->_viewLength / this->_contentLength * scrollBarLength + 0.5;//滑块长度
			double rollTotal = scrollBarLength - this->_sliderLength;//当前滑块可用滑道的总距离
			this->_rollRate = (double)(_contentLength - this->_viewLength) / rollTotal;//滑块每次滚动一次的对应上下文内容的比率
		}
		else {
			this->_sliderLength = scrollBarLength;
			this->_sliderPos = 0;
			this->_offset = 0;
			this->_rollRate = 0;
			this->_overflowLength = 0;
		}
	}
	void ScrollBar::RefreshScroll() {
		SyncInfo();
		ScrollTo(this->_offset, Event::None);
	};
	void ScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		this->_offset += arg.ZDelta * 0.5;
		ScrollTo(this->_offset, Event::OnMouseWheel);
	}
};