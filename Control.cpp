#include "Control.h"
namespace EzUI {

#define UI_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
/*if (_state == ControlState::None && _nowStyle. ##_filed.valid) {\
		return _nowStyle.##_filed;\
	}\*/  \
ControlStyle& style = GetStyle(this->State);\
if(style.##_filed.valid ){\
	return style.##_filed; \
}\
	return this->Style.##_filed;\
}\

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
	void Control::ChildPainting(Controls& controls, PaintEventArgs& args)
	{
		VisibleControls.clear();
		//�����ӿؼ�
		for (auto& it : controls) {
			VisibleControls.push_back(it);
			it->Rending(args);
		}
		//�ӿؼ��������
	}
	void Control::OnPaint(PaintEventArgs& args)
	{
		if (Painting) {
			Painting(args);
		}
		OnBackgroundPaint(args);//�Ȼ��Ʊ���
		OnForePaint(args);//�ٻ���ǰ��

	}

	void Control::OnBackgroundPaint(PaintEventArgs& e)
	{
		Color backgroundColor = GetBackgroundColor();
		HImage backgroundImage = GetBackgroundImage();
		UI_Int radius = GetRadius();
		if (backgroundColor.valid) {
			e.Painter.FillRectangle(Rect{ 0,0,_rect.Width,_rect.Height }, backgroundColor);
		}
		if (backgroundImage.valid && backgroundImage) {
			e.Painter.DrawImage(backgroundImage, Rect{ 0,0,_rect.Width,_rect.Height }, backgroundImage->SizeMode, backgroundImage->Padding);
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		HImage foreImage = GetForeImage();
		if (foreImage.valid && foreImage) {
			e.Painter.DrawImage(foreImage, Rect{ 0,0,_rect.Width,_rect.Height }, foreImage->SizeMode, foreImage->Padding);
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

	ControlStyle& Control::GetStyle(const ControlState& _state) {
		/*if (_state == ControlState::None) {
			return this->_nowStyle;
		}*/
		if (_state == ControlState::Static) {
			return this->Style;
		}
		if (_state == ControlState::Hover) {
			return this->HoverStyle;
		}
		if (_state == ControlState::Active) {
			return this->ActiveStyle;
		}
		if (_state == ControlState::Disable) {
		}
		return this->Style;
	}
	void Control::SetStyleSheet(const EString& styleStr, ControlState _state)
	{
		do
		{
			if (_state == ControlState::Static) {
				this->Style.SetStyleSheet(styleStr);//Ĭ����ʽ
				break;
			}
			if (_state == ControlState::Hover) {
				this->HoverStyle.SetStyleSheet(styleStr);//������ʽ
				break;
			}
			if (_state == ControlState::Active) {
				this->ActiveStyle.SetStyleSheet(styleStr);//��갴����ʽ
				break;
			}
		} while (false);

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
			if (attrName == "rect") {
				this->SetRect(Rect(attrValue));
				break;
			}
			if (attrName == "margin") {//��ѭwebǰ�˵Ĺ���
				auto strs = attrValue.Split(",");
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
		} while (false);
	}
	void Control::OnLoad() {}
	EString Control::GetFontFamily(ControlState _state)
	{
		/*if (_state == ControlState::None && !_nowStyle.FontFamily.empty()) {
			return _nowStyle.FontFamily;
		}*/
		if (_state == ControlState::None) {
			_state = this->State;
		}
		EString _FontFamily;
	loop:
		_FontFamily = this->GetStyle(_state).FontFamily; //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		if (!_FontFamily.empty()) {
			return _FontFamily;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_FontFamily = pControl->GetStyle(_state).FontFamily;
			if (!_FontFamily.empty()) {
				return _FontFamily;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (_FontFamily.empty() && _state != ControlState::Static) {
			_state = ControlState::Static;//����Ӹ���ʽ����Ȼδ�ҵ�,���Ҿ�̬��ʽ
			goto loop;
		}
		return _FontFamily;
	}
	UI_Int  Control::GetFontSize(ControlState _state)
	{
		/*if (_state == ControlState::None && _nowStyle.FontSize.valid) {
			return _nowStyle.FontSize;
		}*/
		if (_state == ControlState::None) {
			_state = this->State;
		}
		UI_Int _FontSize;
	loop:
		_FontSize = this->GetStyle(_state).FontSize; //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		if (_FontSize.valid) {
			return _FontSize;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_FontSize = pControl->GetStyle(_state).FontSize;
			if (_FontSize.valid) {
				return _FontSize;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (!(_FontSize.valid) && _state != ControlState::Static) {
			_state = ControlState::Static;//����Ӹ���ʽ����Ȼδ�ҵ�,���Ҿ�̬��ʽ
			goto loop;
		}
		return _FontSize;
	}
	Color  Control::GetForeColor(ControlState _state)
	{
		/*if (_state == ControlState::None && _nowStyle.ForeColor.valid) {
			return _nowStyle.ForeColor;
		}*/
		if (_state == ControlState::None) {
			_state = this->State;
		}
		Color _ForeColor;
	loop:
		_ForeColor = this->GetStyle(_state).ForeColor; //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		if (_ForeColor.valid) {
			return _ForeColor;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_ForeColor = pControl->GetStyle(_state).ForeColor;
			if (_ForeColor.valid) {
				return _ForeColor;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (!(_ForeColor.valid) && _state != ControlState::Static) {
			_state = ControlState::Static;//����Ӹ���ʽ����Ȼδ�ҵ�,���Ҿ�̬��ʽ
			goto loop;
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
		SetLocation({ X,_rect.Y });
	}
	void Control::SetY(const int& Y) {
		SetLocation({ _rect.X,Y });
	}
	void Control::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, _rect.Width, _rect.Height));
	}
	void Control::SetWidth(const int& width) {
		SetSize({ width,_rect.Height });
	}
	void Control::SetHeight(const int& height) {
		SetSize({ _rect.Width,height });
	}
	void Control::SetSize(const Size& size)
	{
		SetRect({ _rect.X,_rect.Y,size.Width,size.Height });
	}
	const Rect& Control::GetRect()
	{
		return _rect;
	}

	void Control::SetFixedWidth(const int& fixedWidth)
	{
		_fixedWidth = fixedWidth;
		SetRect({ _rect.X,_rect.Y,fixedWidth,_rect.Height });

	}
	void Control::SetFixedHeight(const int& fixedHeight)
	{
		_fixedHeight = fixedHeight;
		SetRect({ _rect.X,_rect.Y,_rect.Width,fixedHeight });
	}
	void Control::SetFixedSize(const Size& size)
	{
		_fixedWidth = size.Width;
		_fixedHeight = size.Height;
		SetRect({ _rect.X,_rect.Y,size.Width,size.Height });
	}
	const int& Control::GetFixedWidth()
	{
		return _fixedWidth;
	}
	const int& Control::GetFixedHeight()
	{
		return _fixedHeight;
	}
	bool Control::CheckEventPassThrough(const Event& eventType)
	{
		if ((MousePassThrough & eventType) == eventType) {
			return true;
		}
		return false;
	}
	bool Control::CheckEventNotify(const Event& eventType)
	{
		if ((_eventNotify & eventType) == eventType) {
			return true;
		}
		return false;
	}

	bool Control::IsPendLayout() {
		return this->_layoutState == LayoutState::Pend;
	}

	void Control::TryPendLayout() {
		if (this->_layoutState == LayoutState::None) {
			this->_layoutState = LayoutState::Pend;
		}
	}
	void Control::EndLayout() {
		this->_layoutState = LayoutState::None;
	}

	Rect Control::GetClientRect() {
		Control* pCtrl = this;
		const Rect& rect = GetRect();
		int x = rect.X;
		int y = rect.Y;
		while ((pCtrl = pCtrl->Parent))
		{
			x += pCtrl->X();
			y += pCtrl->Y();
		}
		return Rect{ x,y,rect.Width,rect.Height };
	}
	void Control::SetRect(const Rect& rect)
	{
		this->_rect = rect;
		if (_fixedWidth) {
			_rect.Width = _fixedWidth;
		}
		if (_fixedHeight) {
			_rect.Height = _fixedHeight;
		}
		if (this->Parent) {
			const Rect& pRect = Parent->GetRect();
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
		}

		Point newLocation = _rect.GetLocation();
		Size newSize = _rect.GetSize();

		if (!_lastLocation.Equals(newLocation)) {
			if (PublicData && CheckEventNotify(Event::OnLocation)) {
				LocationEventArgs args;
				args.PrevLocation = _lastLocation;
				args.Location = newLocation;
				PublicData->Notify(this, args);
			}
			OnLocation(newLocation);
			_lastLocation = newLocation;
		}

		if (!newSize.Equals(_lastSize)) {
			if (PublicData && CheckEventNotify(Event::OnSize)) {
				SizeEventArgs args;
				args.PrevSize = _lastSize;
				args.Size = newSize;
				PublicData->Notify(this, args);
			}
			OnSize(newSize);
			_lastSize = newSize;
		}

	}
	void Control::SetTips(const EString& text)
	{
		_tipsText = text.utf16();
	}
	void Control::ResumeLayout()
	{
		if (this->_layoutState == LayoutState::Layouting) {
			return;
		}
		this->_layoutState = LayoutState::Layouting;//������
		this->OnLayout();
		this->_layoutState = LayoutState::None;//���������Ҫ�����ֱ�־����
	}
	void Control::OnLayout() {
		if (ScrollBar) {//������ڹ����������ù������ľ���λ��
			ScrollBar->OwnerSize({ _rect.Width,_rect.Height });
		}
	}

	//ר�Ŵ��������Ϣ��
	void Control::OnKeyBoardEvent(const KeyboardEventArgs& args) {
		if (PublicData == NULL) return;
		WindowData* winData = PublicData;
#define CONTROL_IN_WINDOW (winData->InputControl || winData->FocusControl)
		bool b1 = this->CheckEventNotify(args.EventType);//�ȼ���Ƿ���֪ͨ��������
		if (!b1) {
			return;
		}
		bool b2 = winData->Notify(this, (KeyboardEventArgs&)args);//���������Ǳ���ô˵
		if (b2) {
			return;
		}
		bool b3 = CONTROL_IN_WINDOW;//����Ƿ�ɾ���˿ؼ�
		if (!b3) {
			return;
		}
		switch (args.EventType)
		{
		case Event::OnChar: {
			OnChar(args.wParam, args.lParam);
			break;
		}
		case Event::OnKeyDown: {
			OnKeyDown(args.wParam, args.lParam);
			break;
		}
		case Event::OnKeyUp: {
			OnKeyUp(args.wParam, args.lParam);
			break;
		}
		default:
			break;
		}
	}

	//ר�Ŵ��������Ϣ��
	void Control::OnMouseEvent(const MouseEventArgs& _args) {
		if (PublicData == NULL) return;
		WindowData* winData = PublicData;
		MouseEventArgs& args = (MouseEventArgs&)_args;

#define CONTROL_IN_WINDOW (winData->InputControl || winData->FocusControl)
		if (CONTROL_IN_WINDOW && CheckEventPassThrough(args.EventType)) {//�����괩͸
			MouseEventArgs copy_args = args;
			copy_args.Location.X += this->X();
			copy_args.Location.Y += this->Y();
			this->Parent->OnMouseEvent(copy_args);//��������˴�͸��ֱ�ӷ��͸���һ��ؼ�
		}

		bool b1 = this->CheckEventNotify(args.EventType);//�ȼ���Ƿ���֪ͨ��������
		if (!b1) {
			return;
		}
		bool b2 = winData->Notify(this, args);//���������Ǳ���ô˵
		if (b2) {
			return;
		}
		bool b3 = CONTROL_IN_WINDOW;//����Ƿ�ɾ���˿ؼ�
		if (!b3) {
			return;
		}

		switch (args.EventType)
		{
		case Event::OnMouseWheel: {
			OnMouseWheel(args.Delta, args.Location);
			break;
		}
		case Event::OnMouseClick: {
			OnMouseClick(args.Button, args.Location);
			break;
		}
		case Event::OnMouseEnter: {
			OnMouseEnter(args.Location);
			break;
		}
		case Event::OnMouseMove: {
			if (!_mouseIn) {
				_mouseIn = true;
				MouseEventArgs copy_args = args;
				copy_args.EventType = Event::OnMouseEnter;
				OnMouseEvent(copy_args);
			}
			if (CONTROL_IN_WINDOW) {
				OnMouseMove(args.Location);
			}
			break;
		}
		case Event::OnMouseDoubleClick: {
			OnMouseDoubleClick(args.Button, args.Location);
			break;
		}
		case Event::OnMouseDown: {
			OnMouseDown(args.Button, args.Location);
			break;
		}
		case Event::OnMouseUp: {
			OnMouseUp(args.Button, args.Location);
			break;
		}
		case Event::OnMouseLeave: {
			_mouseIn = false;
			OnMouseLeave();
			break;
		}
		default:
			break;
		}
	}
	void Control::Rending(PaintEventArgs& args) {
		this->PublicData = args.PublicData;
		if (!Visible) { return; }//����ؼ�����Ϊ���ɼ�ֱ�Ӳ�����

		if (this->IsPendLayout()) {//���Ƶ�ʱ�����ʱ���й���Ĳ��� ����� �����ò�����Ч�����ò��ֱ�־
			this->ResumeLayout();
		}

		auto clientRect = this->GetClientRect();//��ȡ���ڸ����ڵ����
		if (clientRect.IsEmptyArea()) { return; }
		auto& invalidRect = args.InvalidRectangle;
		auto& pt = args.Painter;
		Rect _ClipRect = clientRect;
		this->ComputeClipRect();//���¼�����ڸ��׵Ĳü�����
		if (!Rect::Intersect(_ClipRect, this->ClipRect, invalidRect)) {//���ػ�������вü�
			return;
		}
		if (!_load) {
			OnLoad();
			_load = true;
		}
		pt.Count++;

		_rePaintMtx.lock();
		this->_lastDrawRect = _ClipRect;//��¼���һ�λ��Ƶ�����
		_rePaintMtx.unlock();

		//���û���ƫ��
		pt.SetTransform(clientRect.X, clientRect.Y);

		int r = GetRadius();
		//bool isScrollBar = dynamic_cast<EzUI::ScrollBar*>(this);
		//r = isScrollBar ? 0 : r;//��Ϊ�������ǲ���Ҫ��Բ�ǵ�
#if USED_Direct2D
		if (r > 0) {
			//����Բ�ǿؼ� ʹ������ķ�ʽ (��������Ϊ�˿ؼ��ڲ�������ô���ƶ����ᳬ��Բ�ǲ���) �������
			Geometry roundRect(0, 0, clientRect.Width, clientRect.Height, r);
			Geometry _clientRect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height);
			Geometry outClipRect;
			Geometry::Intersect(outClipRect, roundRect, _clientRect);
			pt.PushLayer(outClipRect);
		}
		else {
			//��Ծ��οؼ�
			pt.PushAxisAlignedClip(Rect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height));
		}
#endif 
		//��ʼ����
		bool isIntercept = false;
		if (CheckEventNotify(Event::OnPaint)) {//��鵱ǰ�¼��Ƿ���Ҫ��֪ͨ��������
			isIntercept = this->PublicData->Notify(this, args);//�����Ǳ��Ƿ���
		}
		if (!isIntercept) {
			this->OnPaint(args);//���ƻ���������
		}
		this->ChildPainting(this->_controls, args);//�����ӿؼ�
		//���ƹ�����
		EzUI::ScrollBar* scrollbar = NULL;
		if (scrollbar = this->ScrollBar) {
			scrollbar->PublicData = args.PublicData;
			Rect barRect = scrollbar->GetClientRect();
			//����ƫ��
			pt.SetTransform(barRect.X, barRect.Y);

			scrollbar->Rending(args);
		}
		//����ƫ��
		pt.SetTransform(clientRect.X, clientRect.Y);
		//���Ʊ߿�
		this->OnBorderPaint(args);//���Ʊ߿�
		//�ָ�ƫ��
		pt.SetTransform(0, 0);

#if USED_Direct2D
		if (r > 0) {
			pt.PopLayer();//����
		}
		else {
			pt.PopAxisAlignedClip();
		}
#endif 
#ifdef DEBUGPAINT
		if (PublicData->Debug) {
			pt.DrawRectangle(GetClientRect(), Color::White);
		}
#endif
	}

	Control::~Control()
	{
		if (_hCursor) {
			::DestroyCursor(_hCursor);
		}
		//���ٿؼ�ǰ���Ƚ��ؼ��Ӹ��������Ƴ�
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
		ctl->PublicData = this->PublicData;
		ctl->Parent = this;
		Size sz{ _rect.Width,_rect.Height };
		//����ؼ���һ�����ɶ���
		if (dynamic_cast<Spacer*>(ctl)) {
			_spacer.push_back(ctl);//�ؼ����ռ����ɶ��� �����ؼ�ִ������������ʱ�� �Զ����ͷſؼ��ڵ��ɶ���
		}

		this->TryPendLayout();//��ӿؼ���Ҫ���������¹���
	}
	ControlIterator Control::RemoveControl(Control* ctl)
	{
		ControlIterator nextIt;
		ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
		if (it1 != _controls.end()) {
			ctl->OnRemove();
			this->TryPendLayout();//�Ƴ��ؼ���Ҫ���������¹���
			nextIt = _controls.erase(it1);
			ControlIterator it2 = ::std::find(VisibleControls.begin(), VisibleControls.end(), ctl);
			if (it2 != VisibleControls.end()) {
				VisibleControls.erase(it2);
			}
		}
		return nextIt;
	}
	void Control::OnRemove()
	{
		for (auto& it : _controls) {
			it->OnRemove();
		}
		if (PublicData) {
			PublicData->DelTips(this);//�Ƴ�tips���ְ�
			PublicData->RemoveControl(this);
			PublicData = NULL;
		}
		_load = false;
		Parent = NULL;
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

	bool Control::IsVisible() {
		bool visible = this->Visible;
		Control* pCtl = this->Parent;
		while (pCtl && visible)
		{
			visible = pCtl->Visible;
			pCtl = pCtl->Parent;
		}
		return visible;
	}

	bool Control::Invalidate() {
		std::unique_lock<std::mutex> autoLock(_rePaintMtx);
		if (PublicData) {
			WindowData* winData = PublicData;
			if (winData) {
				Rect _InvalidateRect = GetClientRect();
				Rect::Union(_InvalidateRect, _lastDrawRect, _InvalidateRect);
				winData->InvalidateRect(&_InvalidateRect);
				return true;
			}
		}
		return false;
	}
	void Control::Refresh() {
		if (Invalidate()) {
			PublicData->UpdateWindow();//��������ȫ����Ч����
		}
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);//���ø��ؼ��Ĳü�����
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->ClipRect);//����͸��ؼ��ԱȽϲü�����
		}
		else {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);//���ø��ؼ��Ĳü�����
			ClipRectRef = this->GetClientRect();
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
			(*i)->OnRemove();
			if (freeControls && !(dynamic_cast<Spacer*>(*i))) {//���ɲ���ɾ�� ���ɱ���ʹ��DestroySpacers()����ɾ��
				delete* i;
			}
		}
		VisibleControls.clear();//��տɼ��ؼ�
		_controls.clear();//����ӿؼ�����
		DestroySpacers();//��յ��ɲ�ɾ������
	}
	void Control::OnChar(WPARAM wParam, LPARAM lParam) {}
	void Control::OnKeyDown(WPARAM wParam, LPARAM lParam) {}
	void Control::OnKeyUp(WPARAM wParam, LPARAM lParam) {}
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
	void Control::Trigger(const MouseEventArgs& args)
	{
		OnMouseEvent(args);
	}
	void Control::Trigger(const KeyboardEventArgs& args)
	{
		OnKeyBoardEvent(args);
	}

	void Control::AddEventNotify(int eventType) {
		_eventNotify = _eventNotify | eventType;
	}
	void Control::RemoveEventNotify(int eventType) {
		_eventNotify = _eventNotify & ~eventType;
	}

	void Control::SetCursor(const EString& fileName) {
		if (_hCursor) {
			::DestroyCursor(_hCursor);
		}
		_hCursor = ::LoadCursorFromFileW(fileName.utf16().c_str());
	}
	HCURSOR Control::GetCursor() {
		if (_hCursor) {
			return _hCursor;
		}
		if (this->Cursor != EzUI::Cursor::None) {
			_hCursor = ::LoadCursor(NULL, (LPTSTR)this->Cursor);
		}
		return _hCursor;
	}

	bool Control::IsRePaint() {
		/* //���淽ʽ,�˷�ʽ���ܻ������ʽ����
		_nowStyle.BackgroundColor = GetBackgroundColor();
		_nowStyle.BackgroundImage = GetBackgroundImage();
		_nowStyle.BorderBottom = GetBorderBottom();
		_nowStyle.BorderColor = GetBorderColor();
		_nowStyle.BorderLeft = GetBorderLeft();
		_nowStyle.BorderRight = GetBorderRight();
		_nowStyle.BorderTop = GetBorderTop();
		_nowStyle.FontFamily = GetFontFamily(this->State);
		_nowStyle.FontSize = GetFontSize(this->State);
		_nowStyle.ForeColor = GetForeColor(this->State);
		_nowStyle.ForeImage = GetForeImage();
		_nowStyle.Radius = GetRadius();
		return _nowStyle.IsValid();*/
		return  true;//  
	}

	void Control::OnMouseEnter(const Point& point)
	{
		this->State = ControlState::Hover;
		if (IsRePaint()) {
			_stateRepaint = true;
			Invalidate();
		}
		if (PublicData) {
			if (!_tipsText.empty()) {//������ʾ����
				PublicData->SetTips(this, _tipsText);
			}
		}
		UI_TRIGGER(MouseEnter, point);
	}
	void Control::OnMouseDown(MouseButton mbtn, const Point& point)
	{
		this->State = ControlState::Active;
		if (IsRePaint()) {
			_stateRepaint = true;
			Invalidate();
		}
		UI_TRIGGER(MouseDown, mbtn, point);
	}
	void Control::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		this->State = ControlState::Hover;
		if (_stateRepaint) {
			Invalidate();
		}
		UI_TRIGGER(MouseUp, mbtn, point);
	}
	void Control::OnMouseLeave()
	{
		this->State = ControlState::Static;
		if (_stateRepaint) {
			_stateRepaint = false;
			Invalidate();
		}
		UI_TRIGGER(MouseLeave);
	}

	void Control::OnLocation(const Point& pt)
	{

	}
	void Control::OnSize(const Size& size)
	{
		__count_onsize++;
		this->TryPendLayout();//���Լ�����
		if (Parent) {
			Parent->TryPendLayout();//�����ؼ�����
		}
	}
	void Control::OnKillFocus()
	{
	}
};