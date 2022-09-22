#include "Control.h"
#include "BoxShadow.h"
namespace EzUI {

#define UI_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed()  { \
if(this->State==ControlState::None){return  Style. ##_filed; }\
ControlStyle &style = GetStyle(this->State); \
if (this->State != ControlState::None &&style. ##_filed.valid) { \
	return style. ##_filed; \
}\
return Style. ##_filed; \
}
	//�����¼���
#define UI_TRIGGER(Event,...)  if( ##Event){ \
Event(this , ##__VA_ARGS__); \
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


	Control::Control() {}
	Control::Control(const Control&) {}
	Control& Control::operator=(const Control&) {
		return *this;
	}
	void Control::OnChildPaint(Controls& controls, PaintEventArgs& args)
	{
		//�����ӿؼ�
		for (auto& it : controls) {
			it->Rending(args);
		}
		//�ӿؼ��������
	}
	void Control::OnPaint(PaintEventArgs& args)
	{
		if (Painting && Painting(args) == true) {
			return;
		}
		OnBackgroundPaint(args);//�Ȼ��Ʊ���
		OnForePaint(args);//�ٻ���ǰ��
		OnChildPaint(this->_controls, args);//�����ӿؼ�
	}
	void Control::OnBackgroundPaint(PaintEventArgs& e)
	{
		if (BackgroundPainting && BackgroundPainting(e) == true) {
			return;
		}
		Color backgroundColor = GetBackgroundColor();
		HImage backgroundImage = GetBackgroundImage();
		UI_Int radius = GetRadius();
		if (backgroundColor.valid) {
			e.Painter.FillRectangle(Rect{ 0,0,_rect.Width,_rect.Height }, backgroundColor);
		}
		if (backgroundImage.valid) {
			e.Painter.DrawImage(backgroundImage.value, Rect{ 0,0,_rect.Width,_rect.Height }, backgroundImage.value->SizeMode, backgroundImage.value->Margin);
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		if (ForePainting && ForePainting(e) == true) {
			return;
		}
		HImage foreImage = GetForeImage();
		if (foreImage.valid) {
			e.Painter.DrawImage(foreImage.value, Rect{ 0,0,_rect.Width,_rect.Height }, foreImage.value->SizeMode, foreImage.value->Margin);
		}
	}
	void Control::OnBorderPaint(PaintEventArgs& e)
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
			e.Painter.DrawRectangle(Rect{ 0,0,_rect.Width,_rect.Height }, borderColor, borderLeft);
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
				this->Style.SetStyleSheet(sheet);//Ĭ����ʽ
				break;
			}
			if (styleStr.find("hover") == 0) {
				this->HoverStyle.SetStyleSheet(sheet);//������ʽ
				break;
			}
			if (styleStr.find("active") == 0) {
				this->ActiveStyle.SetStyleSheet(sheet);//��갴����ʽ
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
				this->SetLocation({ std::stoi(attrValue) ,this->GetRect().Y });
				break;
			}
			if (attrName == "y") {
				this->SetLocation({ this->GetRect().X, std::stoi(attrValue) });
				break;
			}
			if (attrName == "width") {//������������˿���Ǿ��Ǿ��Կ����
				this->SetFixedWidth(std::stoi(attrValue));
				break;
			}
			if (attrName == "height") {//������������˿���Ǿ��Ǿ��Կ����
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
		if (_state != ControlState::None && !style.FontFamily.empty()) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
			return style.FontFamily;
		}
		else if (!Style.FontFamily.empty()) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
			return Style.FontFamily;
		}
		//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
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
		if (_state != ControlState::None && style.FontSize.valid) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
			return style.FontSize;
		}
		else if (Style.FontSize.valid) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
			return Style.FontSize;
		}
		//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
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
		if (_state != ControlState::None && style.ForeColor.valid) { //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
			return style.ForeColor;
		}
		else if (Style.ForeColor.valid) { //�����Ӧ״̬û�� ��Ч�ֶ� ���Ĭ����ʽ������
			return Style.ForeColor;
		}
		//�������û�õ��ʹӸ��ؼ�������ȥ��Ӧ���ֶ�
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
	void Control::SetX(const int& X) {
		SetLocation({ X,Y() });
	}
	void Control::SetY(const int& Y) {
		SetLocation({ X(),Y });
	}
	void Control::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, Width(), Height()));
	}
	void Control::SetWidth(const int& width) {
		SetSize({ width,Height() });
	}
	void Control::SetHeight(const int& height) {
		SetSize({ Width(),height });
	}
	void Control::SetSize(const Size& size)
	{
		SetRect({ X(),Y(),size.Width,size.Height });
	}
	const Rect& Control::GetRect()
	{
		return _rect;
	}
	void Control::OnLayout(const Size& pRect, bool instantly)
	{
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
	void Control::SetFixedWidth(const int& fixedWidth)
	{
		_rect.Width = fixedWidth;
		_fixedWidth = fixedWidth;
	}
	void Control::SetFixedHeight(const int& fixedHeight)
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

	bool Control::CheckEventPassThrough(Event eventType)
	{
		if ((MousePassThrough & eventType) == eventType) {
			return true;
		}
		return false;
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
		this->ComputeClipRect();//����Ҫ���¼�����ڸ��ؼ��Ĳü�����
		OnSize(outSize);//Ȼ��ſ�ʼ������������� //���ֿؼ�������������� ���ӿؼ�����rect
		if (rePaint) {
			Invalidate();
		}
	}
	//ר�Ŵ��������Ϣ��
	void Control::OnMouseEvent(const MouseEventArgs& _args) {
		if (_hWnd == NULL) return;
		WindowData* winData = (WindowData*)UI_GetUserData(_hWnd);
		MouseEventArgs& args = (MouseEventArgs&)_args;
		if (CheckEventPassThrough(args.EventType)) {//�����괩͸
			this->Parent->OnMouseEvent(args);//��������˴�͸��ֱ�ӷ��͸���һ��ؼ�
		}
		switch (args.EventType)
		{
		case Event::OnMouseWheel: {
			if (!winData->Notify(WM_USER + 0x04, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseWheel(args.Delta, args.Location);
			}
			break;
		}
		case Event::OnMouseClick: {
			if (!winData->Notify(WM_USER + 0x05, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseClick(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseEnter: {
			if (!winData->Notify(WM_USER + 0x06, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
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
			if (!winData->Notify(WM_USER + 0x07, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseMove(args.Location);
			}
			break;
		}
		case Event::OnMouseDoubleClick: {
			if (!winData->Notify(WM_USER + 0x08, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseDoubleClick(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseDown: {
			_mouseDown = true;
			if (!winData->Notify(WM_USER + 0x09, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseDown(args.Button, args.Location);
			}
			break;
		}
		case Event::OnMouseUp: {
			bool isDown = _mouseDown;
			_mouseDown = false;
			if (!winData->Notify(WM_USER + 0x0a, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
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
			if (!winData->Notify(WM_USER + 0x0b, (WPARAM)this, (LPARAM)&args)) {//������ڲ������ؾͷ����ؼ�����
				OnMouseLeave();
			}
			break;
		}
		default:
			break;
		}

	}
	void Control::Rending(PaintEventArgs& args) {
		this->_hWnd = args.HWnd;
		if (!_load) {
			OnLoad();
			_load = true;
		}
		if (!Visible) { return; }//����ؼ�����Ϊ���ɼ�ֱ�Ӳ�����
		auto clientRect = this->GetClientRect();//��ȡ���ڸ����ڵ����
		if (clientRect.IsEmptyArea()) { return; }
		auto& invalidRect = args.InvalidRectangle;
		auto& pt = args.Painter;

		Rect _ClipRect = clientRect;
		////���ػ�������вü� ������
		//if (!Rect::Intersect(_ClipRect, _ClipRect, invalidRect)) {
		//	return;
		//}
		//if (Parent) {
		//	//����͸��ؼ��ԱȽϲü�����
		//	if (!Rect::Intersect(_ClipRect, _ClipRect, Parent->GetClientRect())) {
		//		return;
		//	}
		//}
		this->ComputeClipRect();//���¼�����ڸ��׵Ĳü�����
		if (!Rect::Intersect(_ClipRect, this->ClipRect, invalidRect)) {//���ػ�������вü�
			return;
		}
		this->_lastDrawRect = _ClipRect;//��¼���һ�λ��Ƶ�����
		//���û���ƫ��
		pt.OffsetX = clientRect.X; //����ƫ��
		pt.OffsetY = clientRect.Y;//����ƫ��

		if (ShadowWidth > 0) {
			//pt.EndDraw();//�����Direct2D������ô�����Ƚ������� �൱����Flush���浽DC����
			BoxShadow bs(Width(), Height(), ShadowWidth);
			auto sz = bs.GetSize();
			BLENDFUNCTION blendFunc{ 0 };
			blendFunc.SourceConstantAlpha = 255;
			blendFunc.BlendOp = AC_SRC_OVER;
			blendFunc.AlphaFormat = AC_SRC_ALPHA;
			//::AlphaBlend(args., pt.OffsetX - ShadowWidth, pt.OffsetY - ShadowWidth, sz.Width, sz.Height, bs._bufBitmap->GetDC(), 0, 0, sz.Width, sz.Height, blendFunc);
			//pt.BeginDraw();//��������ʣ�µ�����
		}

#if USED_GDIPLUS 
		pt.PushLayer(_ClipRect);
		int r = GetRadius();
#define AntiAlias
#ifdef AntiAlias
		EBitmap* buckBkImg = NULL;//����ǰ�ȱ���һ�µײ�����
		if (r > 0) {
			buckBkImg = new EBitmap(clientRect.Width, clientRect.Height);
			::BitBlt(buckBkImg->GetDC(), 0, 0, clientRect.Width, clientRect.Height, args.DC, clientRect.X, clientRect.Y, SRCCOPY);
		}
#endif

#endif // 

#if USED_Direct2D
		int r = GetRadius();
		if (r > 0) {
			//����Բ�ǿؼ� ʹ������ķ�ʽ (��������Ϊ�˿ؼ��ڲ�������ô���ƶ����ᳬ��Բ�ǲ���) �������
			DxGeometry roundRect(clientRect.X, clientRect.Y, clientRect.Width, clientRect.Height, r);
			DxGeometry clientRect(_ClipRect.X, _ClipRect.Y, _ClipRect.Width, _ClipRect.Height);
			DxGeometry outClipRect;
			DxGeometry::Intersect(outClipRect, roundRect, clientRect);
			pt.PushLayer(outClipRect);
		}
		else {
			//��Ծ��οؼ�
			pt.PushAxisAlignedClip(_ClipRect);
		}
#endif
		//��ʼ����
		this->OnPaint(args);//���ƻ���������

		//�����ֲ� ���� �������ͱ߿򳬳����ؼ�
		//���ƹ�����
		EzUI::ScrollBar* scrollbar = NULL;
		if (scrollbar = this->ScrollBar) {
			scrollbar->_hWnd = args.HWnd;
			Rect barRect = scrollbar->GetClientRect();
			pt.OffsetX = barRect.X; //����ƫ��
			pt.OffsetY = barRect.Y;//����ƫ��
			scrollbar->Rending(args);
		}
		//���Ʊ߿�
		pt.OffsetX = clientRect.X; //����ƫ��
		pt.OffsetY = clientRect.Y;//����ƫ��
		this->OnBorderPaint(args);//���Ʊ߿�

#if USED_GDIPLUS 
#ifdef AntiAlias
		if (r > 0) {
			EBitmap bufBitmap(clientRect.Width, clientRect.Height);//���ƺõ�����
			::BitBlt(bufBitmap.GetDC(), 0, 0, clientRect.Width, clientRect.Height, args.DC, clientRect.X, clientRect.Y, SRCCOPY);//�����ƺõķŵ�bufBitmap����
			GdiplusImage img(bufBitmap._bitmap);
			GdiplusImage* outBitmap = ClipImage(&img, { clientRect.Width, clientRect.Height }, r);
			::BitBlt(args.DC, clientRect.X, clientRect.Y, clientRect.Width, clientRect.Height, buckBkImg->GetDC(), 0, 0, SRCCOPY);//�ѱ��ݵı�������ȥ
			pt.base->DrawImage(outBitmap, clientRect.X, clientRect.Y);//���ü��õ�����ȥ
			delete buckBkImg;
			delete outBitmap;
		}
#undef AntiAlias
#endif
#endif

#if USED_GDIPLUS
		pt.PopLayer();//����
#endif

#if USED_Direct2D
		if (r > 0) {
			pt.PopLayer();//����
		}
		else {
			pt.PopAxisAlignedClip();
		}
#endif

#ifdef DEBUGPAINT
		WindowData* wndData = (WindowData*)UI_GetUserData(_hWnd);
		if (wndData->Debug) {
			pt.DrawRectangle(Rect{ 0,0,_rect.Width,_rect.Height }, Color::White);
		}
#endif
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
		//�ؼ��ͷŵ�ʱ���Զ��ͷŵ���
		for (auto it = _spacer.begin(); it != _spacer.end(); it++)
		{
			this->RemoveControl(*it);
			delete* it;
		}
		_spacer.clear();
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
		//����ӵĿؼ������ȴ��� ����������
		ctl->OnLayout(sz);
		//����ؼ���һ�����ɶ���
		if (dynamic_cast<Spacer*>(ctl)) {
			_spacer.push_back(ctl);//�ؼ����ռ����ɶ��� �����ؼ�ִ������������ʱ�� �Զ����ͷſؼ��ڵ��ɶ���
		}
	}
	ControlIterator Control::RemoveControl(Control* ctl)
	{
		ControlIterator nextIt;
		ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
		if (it1 != _controls.end()) {
			if (::IsWindow(_hWnd)) { //�Ƴ��ؼ�֮ǰ��֪ͨ������
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

	bool Control::Invalidate() {
		if (_hWnd || ::IsWindow(_hWnd)) {
			/*if (Parent) {
				ILayout* l = dynamic_cast<ILayout*>(Parent);
				if (l) {
					l->ResumeLayout();
					return Parent->Invalidate();
				}
			}*/
			WindowData* winData = (WindowData*)UI_GetUserData(_hWnd);
			if (winData) {
				Rect _InvalidateRect;
				Rect::Union(_InvalidateRect, _lastDrawRect, GetClientRect());
				winData->InvalidateRect(&_InvalidateRect);
				return true;
			}
		}
		return false;
	}
	void Control::Refresh() {
		if (Invalidate()) {
			WindowData* winData = (WindowData*)UI_GetUserData(_hWnd);
			winData->UpdateWindow();//��������ȫ����Ч����
		}
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);//���ø��ؼ��Ĳü�����
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->ClipRect);//����͸��ؼ��ԱȽϲü�����
		}
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
			if (::IsWindow(_hWnd)) { //�Ƴ��ؼ�֮ǰ��֪ͨ������
				//Debug::Log("WM_CONTROL_DELETE %p", *i);
				::SendMessage(_hWnd, UI_CONTROL_DELETE, (WPARAM)*i, NULL);
			}

			if (freeControls && !dynamic_cast<Spacer*>(*i)) {//���ɲ���ɾ�� ���ɱ���ʹ��DestroySpacers()����ɾ��
				delete* i;
			}
		}
		VisibleControls.clear();//��տɼ��ؼ�
		_controls.clear();//����ӿؼ�����
		DestroySpacers();//��յ��ɲ�ɾ������
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
			Invalidate();
		}
		else {
			Control* pControl = this->Parent;
			ControlStyle* _style = NULL;
			while (pControl)
			{
				_style = &pControl->HoverStyle;
				if (_style && _style->IsValid()) {
					Invalidate();
					break;
				}
				pControl = pControl->Parent;
			}
		}
		if (Cursor != Cursor::None) {//��������ʱ���ж��Ƿ�������״̬
			_LastCursor = (LPTSTR)::GetClassLongPtr(_hWnd, GCL_HCURSOR);//��¼֮ǰ��״̬
			::SetClassLongPtr(_hWnd, GCL_HCURSOR, (UINT_PTR)::LoadCursor(NULL, (LPTSTR)Cursor));//����״̬
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
			Invalidate();
		}
		else {
			Control* pControl = this->Parent;
			ControlStyle* _style = NULL;
			while (pControl)
			{
				_style = &pControl->ActiveStyle;
				if (_style && _style->IsValid()) {
					Invalidate();
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
			Invalidate();
		}
		else if (this->State != ControlState::None) {
			this->State = ControlState::None;
			Invalidate();
		}
		UI_TRIGGER(MouseUp, mbtn, point);
	}
	void Control::OnMouseLeave()
	{
		if (this->State != ControlState::None) {
			this->State = ControlState::None;//����״̬
			Invalidate();//����״̬֮��ˢ��
		}
		else {
			this->State = ControlState::None;//����뿪������ζ�Ҫ����״̬
		}
		if (_LastCursor) {//����˿ؼ��Ѿ����ù����ָ����ʽ �� ����Ƴ� ��ʱ����Ҫ�ָ���֮ǰ��״̬
			::SetClassLongPtrW(_hWnd, GCL_HCURSOR, (UINT_PTR)_LastCursor.value);
			_LastCursor = NULL;
		}
		UI_TRIGGER(MouseLeave);
	}
	void Control::OnSize(const Size& size)
	{
		for (auto& it : _controls) {
			it->OnLayout(size);//���ӿؼ������������� ���е���rect
		}
	}
	void Control::OnKillFocus()
	{
	}

};