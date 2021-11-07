#include "Control.h"
Control::Control()
{
	ParentWid = NULL;
}

Control::Control(const Control&) {}
Control& Control::operator=(const Control&) {
	return *this;
}

void Control::SetTopmost(bool flag)
{
	_topmost = flag;
}
bool Control::GetTopmost()
{
	return _topmost;
}

#define BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed()  { \
if(this->State==ControlState::None){return  Style. ##_filed; }\
ControlStyle &style = GetStyle(this->State); \
if (this->State != ControlState::None &&style. ##_filed.valid) { \
	return style. ##_filed; \
}\
return Style. ##_filed; \
}

BINDFUNC(UI_Int, Radius);
BINDFUNC(UI_Int, BorderLeft);
BINDFUNC(UI_Int, BorderTop);
BINDFUNC(UI_Int, BorderRight);
BINDFUNC(UI_Int, BorderBottom);

BINDFUNC(Color, BorderColor);
BINDFUNC(Color, BackgroundColor);

BINDFUNC(HImage, ForeImage);
BINDFUNC(HImage, BackgroundImage);


//�����¼���
#define TRIGGER(Event,...)  if( ##Event){ \
Event(this , ##__VA_ARGS__); \
}


void Control::OnChildPaint(Controls &controls, PaintEventArgs &args)
{
	//�����ӿؼ�
	for (auto &it : controls) {
		it->OnEvent(Event::OnPaint, &args);
	}
	//�ӿؼ��������
}
void Control::OnPaint(PaintEventArgs&args)
{
	OnBackgroundPaint(args);//�Ȼ��Ʊ���
	OnForePaint(args);//�ٻ���ǰ��
	OnChildPaint(this->_controls, args);//�����ӿؼ�
}
void Control::OnBackgroundPaint(PaintEventArgs & e)
{
	Color backgroundColor = GetBackgroundColor();
	HImage backgroundImage = GetBackgroundImage();
	UI_Int radius = GetRadius();
	if (backgroundColor.valid) {
		e.Painter.FillRectangle(backgroundColor, Rect{ 0,0,_rect.Width,_rect.Height }, radius);
	}
	if (backgroundImage.valid) {
		e.Painter.DrawImage(backgroundImage, Rect{ 0,0,_rect.Width,_rect.Height }, radius);
	}
}

void Control::OnForePaint(PaintEventArgs&e) {
	HImage foreImage = GetForeImage();
	UI_Int radius = GetRadius();
	if (foreImage.valid) {
		e.Painter.DrawImage(foreImage, Rect{ 0,0,_rect.Width,_rect.Height }, radius);
	}
}

void Control::OnBorderPaint(PaintEventArgs & e)
{
	Color borderColor = GetBorderColor();
	if (!borderColor.valid) return;//�߿���Ч��ɫ������

	UI_Int radius = GetRadius();
	UI_Int borderLeft = GetBorderLeft();
	UI_Int borderTop = GetBorderTop();
	UI_Int borderRight = GetBorderRight();
	UI_Int borderBottom = GetBorderBottom();

	bool hasBorder = borderLeft || borderTop || borderTop || borderBottom;
	if (!hasBorder) return;//�߿�Ϊ0������

	if (radius > 0 && hasBorder) {
		e.Painter.DrawRectangle(borderColor, Rect{ 0,0,_rect.Width,_rect.Height }, borderLeft, radius);
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

ControlStyle& Control::GetStyle(ControlState _state) {
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

void Control::OnLoad()
{

}
EString Control::GetFontFamily(ControlState _state)
{
	ControlStyle &style = GetStyle(_state);
	if (_state != ControlState::None && !style.FontFamily.empty()) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		return style.FontFamily;
	}
	else if (!Style.FontFamily.empty()) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
		return Style.FontFamily;
	}
	//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
	Control *pControl = this->Parent;
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
UI_Float  Control::GetFontSize(ControlState _state)
{
	ControlStyle &style = GetStyle(_state);
	if (_state != ControlState::None &&style.FontSize.valid) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		return style.FontSize;
	}
	else if (Style.FontSize.valid) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
		return Style.FontSize;
	}
	//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
	Control *pControl = this->Parent;
	UI_Float _FontSize;
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
	ControlStyle &style = GetStyle(_state);
	if (_state != ControlState::None &&style.ForeColor.valid) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		return style.ForeColor;
	}
	else if (Style.ForeColor.valid) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
		return Style.ForeColor;
	}
	//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
	Control *pControl = this->Parent;
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
		//if (!Parent->ClipRect.IsEmptyArea()) {
		Rect &ClipRectRef = *(Rect*)(&this->ClipRect);//���ø��ؼ��Ĳü�����
		Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->ClipRect);//����͸��ؼ��԰ɲü�����
	//}
	}
}

//ר�Ŵ��������Ϣ��
void Control::OnMouseEvent(MouseEventArgs &args) {
	switch (args.EventType)
	{
	case Event::OnMouseWheel: {
		if (!::SendMessage(ParentWid, WM_USER + 0x04, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseWheel(args.Delta, args.Location);
		}
		break;
	}
	case Event::OnMouseClick: {
		if (!::SendMessage(ParentWid, WM_USER + 0x05, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseClick(args.Button, args.Location);
		}
		break;
	}
	case Event::OnMouseEnter: {
		if (!::SendMessage(ParentWid, WM_USER + 0x06, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
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
		if (!::SendMessage(ParentWid, WM_USER + 0x07, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseMove(args.Location);
		}
		break;
	}
	case Event::OnMouseDoubleClick: {
		if (!::SendMessage(ParentWid, WM_USER + 0x08, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseDoubleClick(args.Button, args.Location);
		}
		break;
	}
	case Event::OnMouseDown: {
		if (!::SendMessage(ParentWid, WM_USER + 0x09, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseDown(args.Button, args.Location);
		}
		break;
	}
	case Event::OnMouseUp: {

		if (!::SendMessage(ParentWid, WM_USER + 0x0a, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
			OnMouseUp(args.Button, args.Location);
		}
		args.EventType = Event::OnMouseClick;
		OnMouseEvent(args);
		break;
	}
	case Event::OnMouseLeave: {
		_mouseIn = false;
		if (!::SendMessage(ParentWid, WM_USER + 0x0b, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
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
		PaintEventArgs &args = *(PaintEventArgs*)param;
		this->ParentWid = args.HWnd;
		if (!_load) {
			OnLoad();
			_load = true;
		}
		if (!Visible) { return; }//����ؼ�����Ϊ���ɼ�ֱ�Ӳ�����
		auto clientRect = this->GetClientRect();//��ȡ���ڸ����ڵ����
		if (clientRect.IsEmptyArea()) { return; }
		auto &invalidRect = args.InvalidRectangle;
		auto &pt = args.Painter;
		Rect _ClipRect;
		if (!Rect::Intersect(_ClipRect, this->ClipRect, invalidRect)) {//���ػ�������вü�
			return;
		}
		//���û���ƫ��
		pt.OffsetX = clientRect.X; //����ƫ��
		pt.OffsetY = clientRect.Y;//����ƫ��
#if 1

		//int r = 0;
		//if ((r = GetRadius()) > 0) {//Բ�ǿؼ� ʹ������ķ�ʽ (��������Ϊ�˿ؼ��ڲ�������ô���ƶ����ᳬ��Բ�ǲ���)
		//	auto &base = *pt->base;
		//	GraphicsPath gp;//�ؼ�����Ĺ�դ��·��
		//	Region * region = Painter::IntersectRound(clientRect, r, _ClipRect);
		//	base.SetClip(region);//���òü�����
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
		pt.CreateLayer(_ClipRect);// //�ؼ��ڲ����� ��դ�� ��Բ�� �����ڲ���ͼû����radius�Ļ� ���ƻᳬ��Բ�ǲ���,���ǲ��ᳬ�����β���
	//}
#endif // 
			//��ʼ����
		this->OnPaint(args);//���ƻ���������
		//�����ֲ� ���� �������ͱ߿򳬳����ؼ�
		//���ƹ�����
		::ScrollBar *scrollbar = NULL;
		if (scrollbar = this->ScrollBar) {
			Rect barRect = scrollbar->GetClientRect();
			pt.OffsetX = barRect.X; //����ƫ��
			pt.OffsetY = barRect.Y;//����ƫ��
			scrollbar->OnEvent(Event::OnPaint, &args);
		}
		//���Ʊ߿�
		//args.ClipRectangle = parentClipRect;//���ü���������
		pt.OffsetX = clientRect.X; //����ƫ��
		pt.OffsetY = clientRect.Y;//����ƫ��
		this->OnBorderPaint(args);//���Ʊ߿�
#ifdef DEBUGPAINT
		if (this->State == ControlState::Hover) {
			pt.DrawRectangle(Color(255, 0, 0), Rect{ 0,0,_rect.Width,_rect.Height });
		}
		else {
			pt.DrawRectangle(Color(100, 255, 255, 255), Rect{ 0,0,_rect.Width,_rect.Height });
		}
#endif
		pt.PopLayer();//�����ֲ�
		return;
#else
#endif
	}

	default:
		break;
	}
}
void Control::SetRect(const Rect & rect, bool rePaint)
{
	_rect = rect;
	Size outSize{ _rect.Width,_rect.Height };
	if (_fixedWidth) {
		_rect.Width = _fixedWidth;
	}
	if (_fixedHeight) {
		_rect.Height = _fixedHeight;
	}
	this->ComputeClipRect();//����Ҫ���¼�����ڸ��ؼ��Ĳü�����
	OnSize(outSize);//Ȼ��ſ�ʼ������������� //���ֿؼ�������������� ���ӿؼ�����rect
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
const int &Control::GetFixedHeight()
{
	return _fixedHeight;
}
Control::~Control()
{
	if (this->ScrollBar) {
		delete ScrollBar;
	}
	DestroySpacers();
}

void Control::DestroySpacers() {
	//�ؼ��ͷŵ�ʱ���Զ��ͷŵ���
	for (auto it = _spacer.begin(); it != _spacer.end(); it++)
	{
		this->RemoveControl(*it);
		delete *it;
	}
	_spacer.clear();
}


void Control::SetX(int X) {
	Move({ X,Y() });
}
void Control::SetY(int Y) {
	Move({ X(),Y });
}
void Control::Move(const Point&pt) {
	SetRect(Rect(pt.X, pt.Y, Width(), Height()));
	//this->ComputeClipRect();
	//_rect.X = pt.X;
	//_rect.Y = pt.Y;

}

void Control::SetWidth(int width) {
	ReSize({ width,Height() });
}
void Control::SetHeight(int height) {
	ReSize({ Width(),height });
}
void Control::ReSize(const Size & size)
{
	SetRect({ X(),Y(),size.Width,size.Height });
}

void Control::Refresh() {
	if (ParentWid) {
		::SendMessage(ParentWid, WM_CONTROL_REFRESH, (WPARAM)this, NULL);
	}
}
Rect Control::GetClientRect() {
	Control *pCtrl = this;
	int x = _rect.X;
	int y = _rect.Y;
	while (pCtrl = pCtrl->Parent)
	{
		x += pCtrl->X();
		y += pCtrl->Y();
	}
	return Rect{ x,y,_rect.Width,_rect.Height };
}


Rect Control::GetClientRect(const Rect&_rect) {
	Control *pCtrl = this;
	int x = _rect.X;
	int y = _rect.Y;
	while (pCtrl = pCtrl->Parent)
	{
		x += pCtrl->X();
		y += pCtrl->Y();
	}
	return Rect{ x,y,_rect.Width,_rect.Height };
}

int Control::GetAnchorStyle() {

	return _anchorStyle;
}
void Control::SetAnchorStyle(int anchorStyle)
{
	ASSERT(Parent);
	int emptyRect = !_rect.IsEmptyArea();
	ASSERT(emptyRect);
	this->_anchorStyle = anchorStyle;
	Rect 	parentRect = Parent->GetRect();
	_bottom = parentRect.Height - this->Y();
	_right = parentRect.Width - this->X();
	_marginRight = parentRect.Width - this->Width();
	_marginBottom = parentRect.Height - this->Height();
	_isAnchorStyle = true;
}

void Control::AddControl(Control* ctl) {

#ifdef _DEBUG
	for (auto it = _controls.begin(); it != _controls.end(); it++)
	{
		if (*it == ctl) {
			bool repetitive = true;//�����ظ��Ŀؼ����������
			ASSERT(!repetitive);
			return;
		}
	}
#endif // _DEBUG
	_controls.push_back(ctl);
	ctl->ParentWid = this->ParentWid;
	ctl->Parent = this;
	Size sz{ _rect.Width,_rect.Height };
	//����ӵĿؼ������ȴ��� ����������
	ctl->OnLayout(sz);
	//����ؼ���һ�����ɶ���
	if ((DynamicCast(ctl, Spacer, ControlType::ControlSpacer))) {
		_spacer.push_back(ctl);//�ؼ����ռ����ɶ��� �����ؼ�ִ������������ʱ�� �Զ����ͷſؼ��ڵ��ɶ���
	}
}

Control* Control::FindControl(Control * ctl) {
	ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
	if (it1 != _controls.end()) {
		return ctl;
	}
	return NULL;
}

ControlIterator Control::RemoveControl(Control * ctl)
{
	ControlIterator nextIt;
	ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
	if (it1 != _controls.end()) {
		ctl->ParentWid = NULL;
		nextIt = _controls.erase(it1);
		ControlIterator it2 = ::std::find(VisibleControls.begin(), VisibleControls.end(), ctl);
		if (it2 != VisibleControls.end()) {
			VisibleControls.erase(it2);
		}
		if (::IsWindow(ParentWid)) { //�Ƴ��ؼ�֮ǰ��֪ͨ������
			::SendMessage(ParentWid, WM_CONTROL_DELETE, (WPARAM)ctl, NULL);
		}
	}
	return nextIt;

	//ControlIterator returnItor;
	//for (auto it = _controls.begin(); it != _controls.end(); it++)
	//{
	//	if (ctl == *it) {
	//		//�Ƴ��ɼ��ؼ�����
	//		for (auto i = VisibleControls.begin(); i != VisibleControls.end(); i++)
	//		{
	//			if (ctl == *i) {
	//				VisibleControls.erase(i);
	//				break;
	//			}
	//		}
	//		ctl->ParentWid = NULL;
	//		returnItor = _controls.erase(it);
	//		if (::IsWindow(ParentWid)) { //�Ƴ��ؼ�֮ǰ��֪ͨ������
	//			::SendMessage(ParentWid, WM_CONTROL_DELETE, (WPARAM)ctl, NULL);
	//		}
	//		break;
	//	}
	//}
	//return returnItor;
}

UINT_PTR Control::SetTimer(size_t interval)
{
	if (_hasTimer) {
		return (UINT_PTR)this;
	}
	_hasTimer = true;
	return ::SetTimer(ParentWid, (UINT_PTR)this, interval, NULL);
}

void Control::KillTimer() {
	::KillTimer(ParentWid, (UINT_PTR)this);
	_hasTimer = false;
}

Control * Control::FindControl(const EString & objectName)
{
	for (auto &it : *(this->GetControls()))
	{
		if (it->Name == objectName) {
			return it;
		}
		auto ctl = it->FindControl(objectName);
		if (ctl) return ctl;
	}
	return NULL;
}



Controls* Control::GetControls()
{
	return &_controls;
}
void Control::Clear(bool freeControls)
{
	for (auto i = _controls.begin(); i != _controls.end(); i++)
	{
		if (::IsWindow(ParentWid)) { //�Ƴ��ؼ�֮ǰ��֪ͨ������
			::SendMessage(ParentWid, WM_CONTROL_DELETE, (WPARAM)*i, NULL);
		}
		if (freeControls && (*i)->GetType() != ControlType::ControlSpacer) {//���ɲ���ɾ�� ���ɱ���ʹ��DestroySpacers()����ɾ��
			delete *i;
		}
	}
	VisibleControls.clear();//��տɼ��ؼ�
	_controls.clear();//����ӿؼ�����
	DestroySpacers();//��յ��ɲ�ɾ������
}

const ControlType & Control::GetType()
{
	return _Type;
}

void Control::OnChar(WPARAM wParam, LPARAM lParam)
{
}
void Control::OnKeyDown(WPARAM wParam)
{
}
void Control::OnMouseMove(const Point & point)
{
	TRIGGER(MouseMove, point);
}
void Control::OnMouseWheel(short zDelta, const Point & point)
{
	TRIGGER(MouseWheel, zDelta, point);
}
void Control::OnMouseClick(MouseButton mbtn, const Point & point)
{
	TRIGGER(MouseClick, mbtn, point);
}
void Control::OnMouseDoubleClick(MouseButton mbtn, const Point & point)
{
	TRIGGER(MouseDoubleClick, mbtn, point);
}
void Control::OnMouseEnter(const Point & point)
{
	if (HoverStyle.IsValid()) {
		this->State = ControlState::Hover;
		Refresh();
	}
	else {
		Control *pControl = this->Parent;
		ControlStyle *_style = NULL;
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
	if (Cursor.valid) {//��������ʱ���ж��Ƿ�������״̬
		_LastCursor = (LPCSTR)::GetClassLongPtr(ParentWid, GCL_HCURSOR);//��¼֮ǰ��״̬
		::SetClassLongPtr(ParentWid, GCL_HCURSOR, (UINT_PTR)::LoadCursor(NULL, Cursor));//����״̬
	}
#ifdef DEBUGLOG
	//Debug::Log("ClipRect %d %d %d %d", ClipRect.X, ClipRect.Y, ClipRect.Width, ClipRect.Height);
#endif
	TRIGGER(MouseEnter, point);
}

void Control::OnMouseDown(MouseButton mbtn, const Point & point)
{
	this->State = ControlState::Active;
	if (ActiveStyle.IsValid()) {
		Refresh();
	}
	else {
		Control *pControl = this->Parent;
		ControlStyle *_style = NULL;
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

	TRIGGER(MouseDown, mbtn, point);
}
void Control::OnMouseUp(MouseButton mbtn, const Point & point)
{
	if (this->State != ControlState::None&& Rect(0, 0, _rect.Width, _rect.Height).Contains(point)) {
		this->State = ControlState::Hover;
		Refresh();
	}
	else if (this->State != ControlState::None) {
		this->State = ControlState::None;
		Refresh();
	}
	TRIGGER(MouseUp, mbtn, point);
}
void Control::OnMouseLeave()
{
	if (this->State != ControlState::None) {
		this->State = ControlState::None;//����״̬
		Refresh();//����״̬֮��ˢ��
	}
	else {
		this->State = ControlState::None;//����뿪������ζ�Ҫ����״̬
	}
	if (_LastCursor) {//����˿ؼ��Ѿ����ù����ָ����ʽ �� ����Ƴ� ��ʱ����Ҫ�ָ���֮ǰ��״̬
		::SetClassLongPtr(ParentWid, GCL_HCURSOR, (UINT_PTR)_LastCursor.value);
		_LastCursor = NULL;
	}
	TRIGGER(MouseLeave);
}
void Control::OnSize(const Size & size)
{
	for (auto &it : _controls) {
		it->OnLayout(size);//���ӿؼ������������� ���е���rect
	}
}
void Control::OnTimer()
{
}
void Control::OnLayout(const Size&pRect, bool instantly) {
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
	while (_anchorStyle != (AnchorStyle::Top | AnchorStyle::Left))//������Ĭ�ϲ���ʱ
	{
		//���ϱ��� Ĭ�����ϱ���
				//��
		if (_anchorStyle == AnchorStyle::Bottom) {
			_rect.Y = pRect.Height - _bottom;
			break;
		}
		//����
		if (_anchorStyle == (AnchorStyle::Top | AnchorStyle::Bottom)) {
			_rect.Width = _rect.Width;
			_rect.Height = pRect.Height - _marginBottom;
			break;
		}
		//��
		if (_anchorStyle == AnchorStyle::Right) {
			_rect.X = pRect.Width - _right;
			break;
		}
		//��
		if (_anchorStyle == AnchorStyle::Left) {
			_rect.X = pRect.Width - _right;
			break;
		}
		//����
		if (_anchorStyle == (AnchorStyle::Left | AnchorStyle::Right)) {
			_rect.Width = pRect.Width - _marginRight;
			_rect.Height = _rect.Height;
			break;
		}
		//��������
		if (_anchorStyle == (AnchorStyle::Top | AnchorStyle::Bottom | AnchorStyle::Left | AnchorStyle::Right)) {
			_rect.Width = pRect.Width - _marginRight;//�Ż� ��������
			_rect.Height = pRect.Height - _marginBottom;
			break;
		}
		//�������
		//����
		if (_anchorStyle == (AnchorStyle::Top | AnchorStyle::Right)) {
			_rect.X = pRect.Width - _right;
			break;
		}
		//����
		if (_anchorStyle == (AnchorStyle::Bottom | AnchorStyle::Right)) {
			_rect.X = pRect.Width - _right;   //�Ż� �����ұ߾���
			_rect.Y = pRect.Height - _bottom;
			break;
		}
		//����
		if (_anchorStyle == (AnchorStyle::Bottom | AnchorStyle::Left)) {
			_rect.Y = pRect.Height - _bottom;
			break;
		}
		//������
		if (_anchorStyle == (AnchorStyle::Top | AnchorStyle::Bottom | AnchorStyle::Left)) {
			_rect.Height = pRect.Height - _marginBottom;
			break;
		}
		//������
		if (_anchorStyle == (AnchorStyle::Top | AnchorStyle::Bottom | AnchorStyle::Right)) {
			_rect.Height = pRect.Height - _marginBottom;//�Ż� ��������
			_rect.X = pRect.Width - _right;
			break;
		}
		//������
		if (_anchorStyle == (AnchorStyle::Left | AnchorStyle::Right | AnchorStyle::Top)) {
			_rect.Width = pRect.Width - _marginRight;
			break;
		}
		//������
		if (_anchorStyle == (AnchorStyle::Left | AnchorStyle::Right | AnchorStyle::Bottom)) {
			_rect.Width = pRect.Width - _marginRight;//�Ż� ��������
			_rect.Y = pRect.Height - _bottom;
			break;
		}
		break;
	}
	if (instantly) {
		SetRect(_rect);
	}
}