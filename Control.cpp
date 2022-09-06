#include "Control.h"
namespace EzUI {
	Control::Control() {}
	Control::Control(const Control&) {}
	Control& Control::operator=(const Control&) {
		return *this;
	}

#define UI_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed()  { \
if(this->State==ControlState::None){return  Style. ##_filed; }\
ControlStyle &style = GetStyle(this->State); \
if (this->State != ControlState::None &&style. ##_filed.valid) { \
	return style. ##_filed; \
}\
return Style. ##_filed; \
}

	UI_BINDFUNC(UI_Int, Radius);
	UI_BINDFUNC(UI_Int, BorderLeft);
	UI_BINDFUNC(UI_Int, BorderTop);
	UI_BINDFUNC(UI_Int, BorderRight);
	UI_BINDFUNC(UI_Int, BorderBottom);

	UI_BINDFUNC(Color, BorderColor);
	UI_BINDFUNC(Color, BackgroundColor);

	UI_BINDFUNC(HImage, ForeImage);
	UI_BINDFUNC(HImage, BackgroundImage);

	//触发事件宏
#define UI_TRIGGER(Event,...)  if( ##Event){ \
Event(this , ##__VA_ARGS__); \
}
	void Control::OnChildPaint(Controls& controls, PaintEventArgs& args)
	{
		//绘制子控件
		for (auto& it : controls) {
			it->OnEvent(Event::OnPaint, &args);
		}
		//子控件绘制完毕
	}
	void Control::OnPaint(PaintEventArgs& args)
	{
		OnBackgroundPaint(args);//先绘制背景
		OnForePaint(args);//再绘制前景
		OnChildPaint(this->_controls, args);//绘制子控件
	}
	void Control::OnBackgroundPaint(PaintEventArgs& e)
	{
		Color backgroundColor = GetBackgroundColor();
		HImage backgroundImage = GetBackgroundImage();
		UI_Int radius = GetRadius();
		if (backgroundColor.valid) {
			e.Painter.FillRectangle(Rect{ 0,0,_rect.Width,_rect.Height },backgroundColor, radius);
		}
		if (backgroundImage.valid) {
			e.Painter.DrawImage(backgroundImage, Rect{ 0,0,_rect.Width,_rect.Height } , backgroundImage.value->SizeMode, backgroundImage.value->Margin);
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		HImage foreImage = GetForeImage();
		UI_Int radius = GetRadius();
		if (foreImage.valid) {
			e.Painter.DrawImage(foreImage, Rect{ 0,0,_rect.Width,_rect.Height }, foreImage.value->SizeMode, foreImage.value->Margin);
		}
	}
	void Control::OnBorderPaint(PaintEventArgs& e)
	{
		Color borderColor = GetBorderColor();
		if (!borderColor.valid) return;//边框无效颜色不绘制

		UI_Int radius = GetRadius();
		UI_Int borderLeft = GetBorderLeft();
		UI_Int borderTop = GetBorderTop();
		UI_Int borderRight = GetBorderRight();
		UI_Int borderBottom = GetBorderBottom();

		bool hasBorder = borderLeft || borderTop || borderTop || borderBottom;
		if (!hasBorder) return;//边框为0不绘制

		if (radius > 0 && hasBorder) {
			e.Painter.DrawRectangle(Rect{ 0,0,_rect.Width,_rect.Height }, borderColor, borderLeft, radius);
			return;
		}
		if (borderLeft > 0) {
			e.Painter.DrawLine(borderColor, Point{ 0,0 }, Point{ 0,_rect.Height }, borderLeft);
		}
		if (borderTop > 0) {
			e.Painter.DrawLine(borderColor, Point{ 0,0 }, Point{ _rect.Width,0 }, borderTop);
		}
		if (borderRight > 0) {
			e.Painter.DrawLine(borderColor, Point{ _rect.Width,0 }, Point{ _rect.Width,_rect.Height }, borderRight);
		}
		if (borderBottom > 0) {
			e.Painter.DrawLine(borderColor, Point{ 0,_rect.Height }, Point{ _rect.Width,_rect.Height }, borderBottom);
		}
	}

	ControlStyle& Control::GetStyle(ControlState& _state) {
		if (this->Parent && this->Parent->State != ControlState::None) {
			_state = this->Parent->State;
		}
		switch (_state)
		{
		case ControlState::None:
			break;
		case ControlState::Hover:
		{
			return HoverStyle;
		}
		case ControlState::Active:
		{
			return ActiveStyle;
			break;
		}
		case ControlState::Disable:
			break;
		default:
			break;
		}
		return Style;
	}
	void Control::SetStyleSheet(const EString& styleStr)
	{
		__super::SetStyleSheet(styleStr);
		size_t pos1 = styleStr.find("{");
		size_t pos2 = styleStr.find("}");
		auto sheet = styleStr.substr(pos1 + 1, pos2 - pos1 - 1);
		do
		{
			if (pos1 == 0 || pos1 == size_t(-1)) {
				this->Style.SetStyleSheet(sheet);//默认样式
				break;
			}
			if (styleStr.find("hover") == 0) {
				this->HoverStyle.SetStyleSheet(sheet);//悬浮样式
				break;
			}
			if (styleStr.find("active") == 0) {
				this->ActiveStyle.SetStyleSheet(sheet);//鼠标按下样式
				break;
			}
		} while (false);

	}
	void Control::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		__super::SetAttribute(attrName, attrValue);
		do
		{
			if (attrName == "name") {
				this->Name = attrValue;
				break;
			}
			if (attrName == "rect") {
				this->SetRect(Rect(attrValue));
				break;
			}
			if (attrName == "x") {
				this->Move({ std::stoi(attrValue) ,this->GetRect().Y });
				break;
			}
			if (attrName == "y") {
				this->Move({ this->GetRect().X, std::stoi(attrValue) });
				break;
			}
			if (attrName == "width") {//如果单独设置了宽高那就是绝对宽高了
				this->SetFixedWidth(std::stoi(attrValue));
				break;
			}
			if (attrName == "height") {//如果单独设置了宽高那就是绝对宽高了
				this->SetFixedHeight(std::stoi(attrValue));
				break;
			}
			if (attrName == "visible" || attrName == "display") {
				this->Visible = (::strcmp(attrValue.c_str(), "true") == 0 ? true : false);
				break;
			}
			if (attrName == "dock") {
				if (attrValue == "fill") {
					this->Dock = DockStyle::Fill; break;
				}
				if (attrValue == "horizontal") {
					this->Dock = DockStyle::Horizontal; break;
				}
				if (attrValue == "vertical") {
					this->Dock = DockStyle::Vertical; break;
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
				if (attrValue == "move" || attrValue == "movewindow") {
					this->Action = ControlAction::MoveWindow; break;
				}
				break;
			}
			if (attrName == "style") {
				this->Style.SetStyleSheet(attrValue);
				break;
			}
			if (attrName == "hover") {
				this->HoverStyle.SetStyleSheet(attrValue);
				break;
			}
			if (attrName == "active") {
				this->ActiveStyle.SetStyleSheet(attrValue);
				break;
			}
		} while (false);
	}
	void Control::OnLoad() {}
	EString Control::GetFontFamily(ControlState _state)
	{
		ControlStyle& style = GetStyle(_state);
		if (_state != ControlState::None && !style.FontFamily.empty()) { //先看看对应状态的是否有 有效字段
			return style.FontFamily;
		}
		else if (!Style.FontFamily.empty()) { //如果对应状态没有 有效字段 则从默认样式里面拿
			return Style.FontFamily;
		}
		//如果还是没拿到就从父控件里面拿去相应的字段
		Control* pControl = this->Parent;
		EString _FontFamily;
		while (_FontFamily.empty() && pControl)
		{
			_FontFamily = pControl->GetFontFamily(_state);
			if (!_FontFamily.empty()) {
				break;
			}
			pControl = pControl->Parent;
		}
		return _FontFamily;
	}
	UI_Int  Control::GetFontSize(ControlState _state)
	{
		ControlStyle& style = GetStyle(_state);
		if (_state != ControlState::None && style.FontSize.valid) { //先看看对应状态的是否有 有效字段
			return style.FontSize;
		}
		else if (Style.FontSize.valid) { //如果对应状态没有 有效字段 则从默认样式里面拿
			return Style.FontSize;
		}
		//如果还是没拿到就从父控件里面拿去相应的字段
		Control* pControl = this->Parent;
		UI_Int _FontSize;
		while (!_FontSize.valid && pControl)
		{
			_FontSize = pControl->GetFontSize(_state);
			if (_FontSize.valid) {
				break;
			}
			pControl = pControl->Parent;
		}
		return _FontSize;
	}
	Color  Control::GetForeColor(ControlState _state)
	{
		ControlStyle& style = GetStyle(_state);
		if (_state != ControlState::None && style.ForeColor.valid) { //先看看对应状态的是否有 有效字段
			return style.ForeColor;
		}
		else if (Style.ForeColor.valid) { //如果对应状态没有 有效字段 则从默认样式里面拿
			return Style.ForeColor;
		}
		//如果还是没拿到就从父控件里面拿去相应的字段
		Control* pControl = this->Parent;
		Color _ForeColor;
		while (!_ForeColor.valid && pControl)
		{
			_ForeColor = pControl->GetForeColor(_state);
			if (_ForeColor.valid) {
				break;
			}
			pControl = pControl->Parent;
		}
		return _ForeColor;
	}
	const int& Control::X()
	{
		return _rect.X;
	}
	const int& Control::Y()
	{
		return _rect.Y;
	}
	const int& Control::Width()
	{
		return _rect.Width;
	}
	const int& Control::Height()
	{
		return _rect.Height;
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);//引用父控件的裁剪区域
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->ClipRect);//自身和父控件对比较裁剪区域
		}
	}
	bool Control::CheckEventPassThrough(Event eventType) {
		if ((MousePassThrough & eventType) == eventType) {
			return true;
		}
		return false;
	}
	//专门处理鼠标消息的
	void Control::OnMouseEvent(const MouseEventArgs& _args) {
		MouseEventArgs& args = (MouseEventArgs&)_args;
		if (CheckEventPassThrough(args.EventType)) {//检查鼠标穿透
			this->Parent->OnMouseEvent(args);//如果设置了穿透就直接发送给上一层控件
		}
		switch (args.EventType)
		{
		case Event::OnMouseWheel: {
			if (!::SendMessage(_hWnd, WM_USER + 0x04, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseWheel(args.Delta, args.Location);
			}
			break;
		}
		case Event::OnMouseClick: {
			if (!::SendMessage(_hWnd, WM_USER + 0x05, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseClick(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseEnter: {
			if (!::SendMessage(_hWnd, WM_USER + 0x06, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseEnter(args.Location);
			}
			break;
		}
		case Event::OnMouseMove: {
			if (!_mouseIn) {
				args.EventType = Event::OnMouseEnter;
				OnMouseEvent(args);
				_mouseIn = true;
			}
			if (!::SendMessage(_hWnd, WM_USER + 0x07, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseMove(args.Location);
			}
			break;
		}
		case Event::OnMouseDoubleClick: {
			if (!::SendMessage(_hWnd, WM_USER + 0x08, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseDoubleClick(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseDown: {
			_mouseDown = true;
			if (!::SendMessage(_hWnd, WM_USER + 0x09, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseDown(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseUp: {
			bool isDown = _mouseDown;
			_mouseDown = false;
			if (!::SendMessage(_hWnd, WM_USER + 0x0a, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseUp(args.Button, args.Location);
			}
			if (isDown) {
				args.EventType = Event::OnMouseClick;
				OnMouseEvent(args);
			}
			break;
		}
		case Event::OnMouseLeave: {
			_mouseIn = false;
			if (!::SendMessage(_hWnd, WM_USER + 0x0b, (WPARAM)this, (LPARAM)&args)) {//如果窗口不做拦截就发给控件处理
				OnMouseLeave();
			}
			break;
		}
		default:
			break;
		}

	}
	void Control::OnEvent(Event eventType, void* param) {
		switch (eventType)
		{
		case Event::OnPaint: {
#if 1
			PaintEventArgs& args = *(PaintEventArgs*)param;
			this->_hWnd = args.HWnd;
			if (!_load) {
				OnLoad();
				_load = true;
			}
			if (!Visible) { return; }//如果控件设置为不可见直接不绘制
			auto clientRect = this->GetClientRect();//获取基于父窗口的最表
			if (clientRect.IsEmptyArea()) { return; }
			auto& invalidRect = args.InvalidRectangle;
			auto& pt = args.Painter;
			Rect _ClipRect;
			if (!Rect::Intersect(_ClipRect, this->ClipRect, invalidRect)) {//和重绘区域进行裁剪
				return;
			}
			//设置绘制偏移
			pt.OffsetX = clientRect.X; //设置偏移
			pt.OffsetY = clientRect.Y;//设置偏移
#if 1

		//int r = 0;
		//if ((r = GetRadius()) > 0) {//圆角控件 使用纹理的方式 (这样做是为了控件内部无论怎么绘制都不会超出圆角部分)
		//	auto &base = *pt->base;
		//	GraphicsPath gp;//控件本身的光栅化路径
		//	Region * region = Painter::IntersectRound(clientRect, r, _ClipRect);
		//	base.SetClip(region);//设置裁剪区域
		//	delete region;
		//	/*BYTE *buf = new BYTE[region1.GetDataSize()]{0};
		//	region1.GetData(buf, region1.GetDataSize());*/
		//	/*Rect rectf;
		//	region1.GetBounds(&rectf, pt->base);
		//	int a = 0;
		//	SolidBrush sb(Color::White);
		//	pt->base->FillRectangle(&sb, rectf);*/
		//}
		//else {
			pt.CreateLayer(_ClipRect);// //控件内部进行 光栅化 做圆角 但是内部绘图没传入radius的话 绘制会超出圆角部分,但是不会超出矩形部分
		//}
#endif // 
			//开始绘制
			this->OnPaint(args);//绘制基本上下文
			//创建分层 避免 滚动条和边框超出本控件
			//绘制滚动条
			EzUI::ScrollBar* scrollbar = NULL;
			if (scrollbar = this->ScrollBar) {
				scrollbar->_hWnd = args.HWnd;
				Rect barRect = scrollbar->GetClientRect();
				pt.OffsetX = barRect.X; //设置偏移
				pt.OffsetY = barRect.Y;//设置偏移
				scrollbar->OnEvent(Event::OnPaint, &args);
			}
			//绘制边框
			//args.ClipRectangle = parentClipRect;//将裁剪区域重置
			pt.OffsetX = clientRect.X; //设置偏移
			pt.OffsetY = clientRect.Y;//设置偏移
			this->OnBorderPaint(args);//绘制边框

#ifdef DEBUGPAINT
			WindowData* wndData = (WindowData*)UI_GetUserData(_hWnd);
			if (wndData->Debug) {
				/*auto tempRect = GetClientRect();
				tempRect.Width = 5;
				tempRect.Height = 5;
				int size = tempRect.Width * tempRect.Height;
				unsigned long long r = 0;
				unsigned long long g = 0;
				unsigned long long b = 0;
				unsigned long long pos = 0;
				for (size_t y = 0; y < tempRect.GetBottom(); y++)
				{
					for (size_t x = 0; x < tempRect.GetRight(); x++)
					{
						COLORREF  color = ::GetPixel(args.Painter.DC, x, y);
						r += GetRValue(color);
						g += GetGValue(color);
						b += GetBValue(color);
						pos++;
					}
				}
				double r1 = r * 1.0 / size;
				double g1 = g * 1.0 / size;
				double b1 = b * 1.0 / size;*/

				auto bkColor = GetBackgroundColor();
				Color color(255 - bkColor.GetR(), 255 - bkColor.GetG(), 255 - bkColor.GetB());

				if (this->State == ControlState::Hover) {
					pt.DrawRectangle(Rect{ 0,0,_rect.Width,_rect.Height },color);
				}
				else {
					pt.DrawRectangle(Rect{ 0,0,_rect.Width,_rect.Height },color);
				}
				if (!Name.empty()) {
					pt.DrawString(Name, GetFontFamily(), GetFontSize(), color,_rect, TextAlign::MiddleLeft);
				}
			}
#endif
			pt.PopLayer();//弹出分层
			return;
#else
#endif
		}
		default:
			break;
		}
	}
	void Control::SetRect(const Rect& rect, bool rePaint)
	{
		_rect = rect;
		Size outSize{ _rect.Width,_rect.Height };
		if (_fixedWidth) {
			_rect.Width = _fixedWidth;
		}
		if (_fixedHeight) {
			_rect.Height = _fixedHeight;
		}
		this->ComputeClipRect();//这里要重新计算基于父控件的裁剪区域
		OnSize(outSize);//然后才开始触发自身的特性 //布局控件会重载这个函数 对子控件调整rect
		if (rePaint) {
			Refresh();
		}
	}
	const Rect& Control::GetRect()
	{
		return _rect;
	}
	void Control::SetFixedWidth(int fixedWidth)
	{
		_rect.Width = fixedWidth;
		_fixedWidth = fixedWidth;
	}
	void Control::SetFixedHeight(int fixedHeight)
	{
		_rect.Height = fixedHeight;
		_fixedHeight = fixedHeight;
	}
	const int& Control::GetFixedWidth()
	{
		return _fixedWidth;
	}
	const int& Control::GetFixedHeight()
	{
		return _fixedHeight;
	}
	Control::~Control()
	{
		//if (Parent) {
		//	Parent->RemoveControl(this);
		//}
		if (this->ScrollBar) {
			delete ScrollBar;
		}
		DestroySpacers();
	}
	void Control::DestroySpacers() {
		//控件释放的时候自动释放弹簧
		for (auto it = _spacer.begin(); it != _spacer.end(); it++)
		{
			this->RemoveControl(*it);
			delete* it;
		}
		_spacer.clear();
	}
	void Control::SetX(int X) {
		Move({ X,Y() });
	}
	void Control::SetY(int Y) {
		Move({ X(),Y });
	}
	void Control::Move(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, Width(), Height()));
	}
	void Control::SetWidth(int width) {
		ReSize({ width,Height() });
	}
	void Control::SetHeight(int height) {
		ReSize({ Width(),height });
	}
	void Control::ReSize(const Size& size)
	{
		SetRect({ X(),Y(),size.Width,size.Height });
	}
	void Control::Refresh() {
		if (_hWnd) {
			//WindowData* winData = (WindowData*)UI_GetUserData(_hWnd);
			::SendMessage(_hWnd, UI_CONTROL_REFRESH, (WPARAM)this, NULL);
		}
	}
	Rect Control::GetClientRect() {
		Control* pCtrl = this;
		int x = _rect.X;
		int y = _rect.Y;
		while ((pCtrl = pCtrl->Parent))
		{
			x += pCtrl->X();
			y += pCtrl->Y();
		}
		return Rect{ x,y,_rect.Width,_rect.Height };
	}
	size_t Control::Index()
	{
		Controls& pControls = Parent->GetControls();
		size_t pos(0);
		for (auto i = pControls.begin(); i != pControls.end(); i++)
		{
			if (dynamic_cast<Spacer*>(*i)) {
				continue;
			}
			if (*i == this) {
				return pos;
			}
			pos++;
		}
		return size_t(-1);
	}
	void Control::AddControl(Control* ctl) {
		_controls.push_back(ctl);
		ctl->_hWnd = this->_hWnd;
		ctl->Parent = this;
		Size sz{ _rect.Width,_rect.Height };
		//新添加的控件必须先触发 自身布局特性
		ctl->OnLayout(sz);
		//如果控件是一个弹簧对象
		if (dynamic_cast<Spacer*>(ctl)) {
			_spacer.push_back(ctl);//控件内收集弹簧对象 当本控件执行析构函数的时候 自动会释放控件内弹簧对象
		}
	}
	ControlIterator Control::RemoveControl(Control* ctl)
	{
		ControlIterator nextIt;
		ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
		if (it1 != _controls.end()) {
			if (::IsWindow(_hWnd)) { //移除控件之前先通知父窗口
				::SendMessage(_hWnd, UI_CONTROL_DELETE, (WPARAM)ctl, NULL);
			}
			ctl->_hWnd = NULL;
			ctl->Parent = NULL;
			nextIt = _controls.erase(it1);
			ControlIterator it2 = ::std::find(VisibleControls.begin(), VisibleControls.end(), ctl);
			if (it2 != VisibleControls.end()) {
				VisibleControls.erase(it2);
			}
		}
		return nextIt;
	}
	Control* Control::FindControl(const EString& objectName)
	{
		if (this->Name == objectName) {
			return this;
		}
		for (auto& it : (this->GetControls()))
		{
			if (it->Name == objectName) {
				return it;
			}
			auto ctl = it->FindControl(objectName);
			if (ctl) return ctl;
		}
		return NULL;
	}
	Controls Control::FindControl(const EString& attr, const EString& attrValue)
	{
		Controls ctls;
		for (auto& it : (this->GetControls()))
		{
			if (it->GetAttribute(attr) == attrValue) {
				ctls.push_back(it);
			}
			Controls _ctls = it->FindControl(attr, attrValue);
			for (auto& it2 : _ctls) {
				ctls.push_back(it2);
			}
		}
		return ctls;
	}
	Controls& Control::GetControls()
	{
		return _controls;
	}
	Control* Control::GetControl(size_t pos)
	{
		size_t _pos = 0;
		for (auto& it : _controls) {
			if (_pos == pos) {
				return it;
			}
			_pos++;
		}
		return NULL;
	}
	void Control::Clear(bool freeControls)
	{
		Controls temp = _controls;
		for (auto i = temp.begin(); i != temp.end(); i++)
		{
			if (::IsWindow(_hWnd)) { //移除控件之前先通知父窗口
				//Debug::Log("WM_CONTROL_DELETE %p", *i);
				::SendMessage(_hWnd, UI_CONTROL_DELETE, (WPARAM)*i, NULL);
			}

			if (freeControls && !dynamic_cast<Spacer*>(*i)) {//弹簧不能删除 弹簧必须使用DestroySpacers()函数删除
				delete* i;
			}
		}
		VisibleControls.clear();//清空可见控件
		_controls.clear();//清空子控件集合
		DestroySpacers();//清空弹簧并删除弹簧
	}
	void Control::OnChar(WPARAM wParam, LPARAM lParam) {}
	void Control::OnKeyDown(WPARAM wParam) {}
	void Control::OnMouseMove(const Point& point)
	{
		UI_TRIGGER(MouseMove, point);
	}
	void Control::OnMouseWheel(short zDelta, const Point& point)
	{
		UI_TRIGGER(MouseWheel, zDelta, point);
	}
	void Control::OnMouseClick(MouseButton mbtn, const Point& point)
	{
		UI_TRIGGER(MouseClick, mbtn, point);
	}
	void Control::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
	{
		UI_TRIGGER(MouseDoubleClick, mbtn, point);
	}
	void Control::OnMouseEnter(const Point& point)
	{
		if (HoverStyle.IsValid()) {
			this->State = ControlState::Hover;
			Refresh();
		}
		else {
			Control* pControl = this->Parent;
			ControlStyle* _style = NULL;
			while (pControl)
			{
				_style = &pControl->HoverStyle;
				if (_style && _style->IsValid()) {
					Refresh();
					break;
				}
				pControl = pControl->Parent;
			}
		}
		if (Cursor != Cursor::None) {//鼠标移入的时候判断是否有设置状态
			_LastCursor = (LPTSTR)::GetClassLongPtr(_hWnd, GCL_HCURSOR);//记录之前的状态
			::SetClassLongPtr(_hWnd, GCL_HCURSOR, (UINT_PTR)::LoadCursor(NULL, (LPTSTR)Cursor));//设置状态
		}
		UI_TRIGGER(MouseEnter, point);
	}
	void Control::Trigger(const MouseEventArgs& args)
	{
		OnMouseEvent(args);
	}
	void Control::OnMouseDown(MouseButton mbtn, const Point& point)
	{
		this->State = ControlState::Active;
		if (ActiveStyle.IsValid()) {
			Refresh();
		}
		else {
			Control* pControl = this->Parent;
			ControlStyle* _style = NULL;
			while (pControl)
			{
				_style = &pControl->ActiveStyle;
				if (_style && _style->IsValid()) {
					Refresh();
					break;
				}
				pControl = pControl->Parent;
			}
		}
		UI_TRIGGER(MouseDown, mbtn, point);
	}
	void Control::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		if (this->State != ControlState::None && Rect(0, 0, _rect.Width, _rect.Height).Contains(point)) {
			this->State = ControlState::Hover;
			Refresh();
		}
		else if (this->State != ControlState::None) {
			this->State = ControlState::None;
			Refresh();
		}
		UI_TRIGGER(MouseUp, mbtn, point);
	}
	void Control::OnMouseLeave()
	{
		if (this->State != ControlState::None) {
			this->State = ControlState::None;//重置状态
			Refresh();//重置状态之后刷新
		}
		else {
			this->State = ControlState::None;//鼠标离开无论如何都要重置状态
		}
		if (_LastCursor) {//如果此控件已经设置过鼠标指针样式 则 鼠标移出 的时候需要恢复成之前的状态
			::SetClassLongPtrW(_hWnd, GCL_HCURSOR, (UINT_PTR)_LastCursor.value);
			_LastCursor = NULL;
		}
		UI_TRIGGER(MouseLeave);
	}
	void Control::OnSize(const Size& size)
	{
		for (auto& it : _controls) {
			it->OnLayout(size);//让子控件触发布局特性 进行调整rect
		}
	}
	void Control::OnKillFocus()
	{
	}
	void Control::OnLayout(const Size& pRect, bool instantly) {
		while (Dock != DockStyle::None)
		{
			if (Dock == DockStyle::Fill) {
				_rect = { 0,0,pRect.Width,pRect.Height };
				break;
			}
			if (Dock == DockStyle::Vertical) {
				_rect = { X(),0,Width(),pRect.Height };
				break;
			}
			if (Dock == DockStyle::Horizontal) {
				_rect = { 0,Y(),pRect.Width,Height() };
				break;
			}
			break;
		}
		if (instantly) {
			SetRect(_rect);
		}
	}
};