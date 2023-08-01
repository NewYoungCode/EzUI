#include "Control.h"
namespace EzUI {
	bool __IsValid(const int& value) {
		return value != 0;
	}
	bool __IsValid(const Image* value) {
		return value != NULL;
	}
	bool __IsValid(const Color& value) {
		return value.GetValue() != 0;
	}
	bool __IsValid(const EString& value) {
		return !value.empty();
	}
	bool __IsValid(const std::wstring& value) {
		return !value.empty();
	}
#define UI_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
/*�ȸ��ݶ�Ӧ��״̬����ȡ��ʽ */ \
ControlStyle& stateStyle = this->GetStyle(this->State);\
if(__IsValid(stateStyle.##_filed)){\
	return stateStyle.##_filed; \
}\
/*��ȡ��ͬ�Ŀؼ���Ĭ����ʽ */ \
	ControlStyle& defaultStyle = this->GetDefaultStyle(); \
if(__IsValid(defaultStyle.##_filed)){\
return  defaultStyle.##_filed;\
\
}\
/*����������ʽ��δ��ȡ�ɹ�������²Ų��ô���ʽ*/ \
	return this->Style.##_filed;\
}\


#define UI_BORDER_BINDFUNC(_type,_filed1,_filed)  _type Control:: ##Get ##_filed1 ##_filed(ControlState _state)  { \
/*�ȸ��ݶ�Ӧ��״̬����ȡ��ʽ */ \
ControlStyle& stateStyle = this->GetStyle(this->State);\
if(__IsValid(stateStyle .##_filed1. ##_filed)){\
	return stateStyle .##_filed1.##_filed; \
}\
/*��ȡ��ͬ�Ŀؼ���Ĭ����ʽ */ \
	ControlStyle& defaultStyle = this->GetDefaultStyle(); \
if(__IsValid(defaultStyle .##_filed1.##_filed)){\
return  defaultStyle .##_filed1.##_filed;\
\
}\
/*����������ʽ��δ��ȡ�ɹ�������²Ų��ô���ʽ*/ \
	return this->Style .##_filed1.##_filed;\
}\

	UI_BORDER_BINDFUNC(int, Border, TopLeftRadius);
	UI_BORDER_BINDFUNC(int, Border, TopRightRadius);
	UI_BORDER_BINDFUNC(int, Border, BottomRightRadius);
	UI_BORDER_BINDFUNC(int, Border, BottomLeftRadius);
	UI_BORDER_BINDFUNC(int, Border, Left);
	UI_BORDER_BINDFUNC(int, Border, Top);
	UI_BORDER_BINDFUNC(int, Border, Right);
	UI_BORDER_BINDFUNC(int, Border, Bottom);
	UI_BORDER_BINDFUNC(Color, Border, Color);

	UI_BINDFUNC(Color, BackColor);
	UI_BINDFUNC(Image*, ForeImage);
	UI_BINDFUNC(Image*, BackImage);

	Control::Control() {}

	void Control::OnChildPaint(PaintEventArgs& args)
	{
		VisibleControls = GetControls();
		//�����ӿؼ�
		for (auto& it : VisibleControls) {
			it->DoPaint(args);
		}
		//�ӿؼ��������
		//����ƫ�� �����ö�����
		if (args.OffSetPoint.size() > 0) {
			Point& offset = *(args.OffSetPoint.rbegin());
			args.Graphics.SetTransform(offset.X, offset.Y);
		}
	}

	void Control::OnPaint(PaintEventArgs& args)
	{
		if (this->EventHandler) {
			if (this->EventHandler(this, args)) {
				return;
			}
		}
		OnBackgroundPaint(args);//�Ȼ��Ʊ���
		OnForePaint(args);//�ٻ���ǰ��
	}

	void Control::OnBackgroundPaint(PaintEventArgs& e)
	{
		const Color& backgroundColor = this->GetBackColor();
		Image* backgroundImage = this->GetBackImage();

		if (backgroundColor.GetValue() != 0) {
			e.Graphics.SetColor(backgroundColor);
			e.Graphics.FillRectangle(Rect{ 0,0,_rect.Width,_rect.Height });
		}
		if (backgroundImage != NULL) {
			e.Graphics.DrawImage(backgroundImage, Rect{ 0,0,_rect.Width,_rect.Height }, backgroundImage->SizeMode, backgroundImage->Padding);
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		Image* foreImage = this->GetForeImage();
		if (foreImage) {
			e.Graphics.DrawImage(foreImage, Rect{ 0,0,_rect.Width,_rect.Height }, foreImage->SizeMode, foreImage->Padding);
		}
	}
	void Control::OnBorderPaint(PaintEventArgs& e, const Border& border)
	{
		const Color& borderColor = border.Color;
		if (borderColor.GetValue() == 0) return;//�߿���Ч��ɫ������
		const int& borderLeft = border.Left;
		const int& borderTop = border.Top;
		const int& borderRight = border.Right;
		const int& borderBottom = border.Bottom;
		const int& topLeftRadius = border.TopLeftRadius;
		const int& topRightRadius = border.TopRightRadius;
		const int& bottomRightRadius = border.BottomRightRadius;
		const int& bottomLeftRadius = border.BottomLeftRadius;
		//����ľ���
		if (topLeftRadius == 0 || topRightRadius == 0 && bottomLeftRadius == 0 && bottomRightRadius == 0) {
			bool hasBorder = borderLeft || borderTop || borderRight || borderBottom;
			if (!hasBorder) return;//�߿�Ϊ0������
			e.Graphics.SetColor(borderColor);
			if (borderLeft > 0) {
				e.Graphics.DrawLine(Point{ 0,0 }, Point{ 0,_rect.Height }, borderLeft);
			}
			if (borderTop > 0) {
				e.Graphics.DrawLine(Point{ 0,0 }, Point{ _rect.Width,0 }, borderTop);
			}
			if (borderRight > 0) {
				e.Graphics.DrawLine(Point{ _rect.Width,0 }, Point{ _rect.Width,_rect.Height }, borderRight);
			}
			if (borderBottom > 0) {
				e.Graphics.DrawLine(Point{ 0,_rect.Height }, Point{ _rect.Width,_rect.Height }, borderBottom);
			}
		}
		else {
			int value1 = borderLeft > borderTop ? borderLeft : borderTop;
			int value2 = borderRight > borderBottom ? borderRight : borderBottom;
			int maxBorder = value1 > value2 ? value1 : value2;
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
				this->SetRect(Convert::StringToRect(attrValue));
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
				this->_visible = (::strcmp(attrValue.c_str(), "true") == 0 ? true : false);
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

		} while (false);
	}
	std::wstring Control::GetFontFamily(ControlState _state)
	{
		/*if (_state == ControlState::None && !_nowStyle.FontFamily.empty()) {
			return _nowStyle.FontFamily;
		}*/
		if (_state == ControlState::None) {
			_state = this->State;
		}
		std::wstring _FontFamily;
	loop:
		_FontFamily = this->GetStyle(_state).FontFamily; //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		if (__IsValid(_FontFamily)) {
			return _FontFamily;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_FontFamily = pControl->GetStyle(_state).FontFamily;
			if (__IsValid(_FontFamily)) {
				return _FontFamily;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (!__IsValid(_FontFamily) && _state != ControlState::Static) {
			_state = ControlState::Static;//����Ӹ���ʽ����Ȼδ�ҵ�,���Ҿ�̬��ʽ
			goto loop;
		}
		return _FontFamily;
	}
	int  Control::GetFontSize(ControlState _state)
	{
		/*if (_state == ControlState::None && _nowStyle.FontSize.valid) {
			return _nowStyle.FontSize;
		}*/
		if (_state == ControlState::None) {
			_state = this->State;
		}
		int _FontSize;
	loop:
		_FontSize = this->GetStyle(_state).FontSize; //�ȿ�����Ӧ״̬���Ƿ��� ��Ч�ֶ�
		if (__IsValid(_FontSize)) {
			return _FontSize;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_FontSize = pControl->GetStyle(_state).FontSize;
			if (__IsValid(_FontSize)) {
				return _FontSize;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (!__IsValid(_FontSize) && _state != ControlState::Static) {
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
		if (__IsValid(_ForeColor)) {
			return _ForeColor;//�����ǰ�ؼ�������ҵ��ͷ���
		}
		Control* pControl = this->Parent;
		while (pControl)//���û����Ӹ��ؼ�������Ҷ�Ӧ����ʽ
		{
			_ForeColor = pControl->GetStyle(_state).ForeColor;
			if (__IsValid(_ForeColor)) {
				return _ForeColor;//����Ӹ��ؼ�������ҵ��ͷ���
			}
			pControl = pControl->Parent;
		}
		if (!__IsValid(_ForeColor) && _state != ControlState::Static) {
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
		_fixedSize.Width = fixedWidth;
		SetRect({ _rect.X,_rect.Y,fixedWidth,_rect.Height });
	}
	void Control::SetFixedHeight(const int& fixedHeight)
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
	const int& Control::GetFixedWidth()
	{
		return _fixedSize.Width;
	}
	const int& Control::GetFixedHeight()
	{
		return _fixedSize.Height;
	}
	bool Control::CheckEventPassThrough(const Event& eventType)
	{
		if ((MousePassThrough & eventType) == eventType) {
			return true;
		}
		return false;
	}

	bool Control::IsPendLayout() {
		return this->_layoutState == LayoutState::Pend;
	}

	const LayoutState& Control::TryPendLayout() {
		if (this->_layoutState == LayoutState::None) {
			this->_layoutState = LayoutState::Pend;
		}
		return this->_layoutState;
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
	const DockStyle& Control::GetDockStyle()
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
		while (_dock != DockStyle::None && this->Parent)
		{
			const Rect& pRect = Parent->GetRect();
			if (_dock == DockStyle::Fill) {
				_rect = Rect{ 0,0,pRect.Width,pRect.Height };
				break;
			}
			if (_dock == DockStyle::Vertical) {
				_rect = Rect{ X(),0,Width(),pRect.Height };
				break;
			}
			if (_dock == DockStyle::Horizontal) {
				_rect = Rect{ 0,Y(),pRect.Width,Height() };
				break;
			}
			break;
		}

		Point newLocation = _rect.GetLocation();
		Size newSize = _rect.GetSize();

		bool onRect = false;
		if (!_lastLocation.Equals(newLocation)) {
			OnLocation(newLocation);
			_lastLocation = newLocation;
			onRect = true;
		}
		if (!newSize.Equals(_lastSize)) {
			this->TryPendLayout();//���Լ�����
			if (Parent) {
				Parent->TryPendLayout();//�����ؼ�����
			}
			OnSize(newSize);
			_lastSize = newSize;
			onRect = true;
		}
		if (onRect) {
			OnRect(GetRect());
		}
		return this->_rect;
	}
	void Control::SetTips(const EString& text)
	{
		_tipsText = text.utf16();
	}
	void Control::ResumeLayout()
	{
		if (this->_layoutState == LayoutState::Layouting || this->_layoutState == LayoutState::None) {
			return;
		}
		this->_layoutState = LayoutState::Layouting;//������
		if (GetScrollBar()) {//������ڹ����������ù������ľ���λ��
			GetScrollBar()->OWnerSize({ _rect.Width,_rect.Height });
		}
		this->OnLayout();
		this->_layoutState = LayoutState::None;//���������Ҫ�����ֱ�־����
	}
	void Control::OnLayout() {
		this->EndLayout();
	}

	//ר�Ŵ��������Ϣ��
	bool Control::OnKeyBoardEvent(const KeyboardEventArgs& _args) {
		bool isRemove = false;
		this->_isRemove = &isRemove;
		KeyboardEventArgs& args = (KeyboardEventArgs&)_args;
		do
		{
			if (Enable == false) break;
			if (PublicData == NULL) break;
			WindowData* winData = PublicData;
			if (CheckEventPassThrough(args.EventType) && this->Parent) {//�����괩͸
				KeyboardEventArgs copy_args = args;
				this->Parent->OnKeyBoardEvent(copy_args);//��������˴�͸��ֱ�ӷ��͸���һ��ؼ�
			}
			//�ȼ���Ƿ���֪ͨ��������
			//֪ͨ���������ڵ�OnNotify���� ���д���
			bool b2 = winData->Notify(this, args);
			//����Ƿ�ɾ���˿ؼ�
			if (b2 || isRemove) {
				break;
			}
			switch (args.EventType)
			{
			case Event::OnKeyChar: {
				OnKeyChar(args.wParam, args.lParam);
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
		} while (false);
		if (!isRemove) {
			if (this->EventHandler) {
				this->EventHandler(this, args);
			}
			if (!isRemove) {
				this->_isRemove = NULL;
				return true;
			};
		}
		return false;
	}

	//ר�Ŵ��������Ϣ��
	bool Control::OnMouseEvent(const MouseEventArgs& _args) {
		bool isRemove = false;
		this->_isRemove = &isRemove;
		MouseEventArgs& args = (MouseEventArgs&)_args;
		do
		{
			if (Enable == false) break;
			if (PublicData == NULL) break;
			WindowData* winData = PublicData;
			if (CheckEventPassThrough(args.EventType) && this->Parent) {//�����괩͸
				MouseEventArgs copy_args = args;
				copy_args.Location.X += this->X();
				copy_args.Location.Y += this->Y();
				this->Parent->OnMouseEvent(copy_args);//��������˴�͸��ֱ�ӷ��͸���һ��ؼ�
			}
			//֪ͨ���������ڵ�OnNotify���� ���д���
			bool b2 = winData->Notify(this, args);;
			//����Ƿ�ɾ���˿ؼ�
			if (b2 || isRemove) {
				break;
			}

			switch (args.EventType)
			{
			case Event::OnMouseWheel: {
				OnMouseWheel(args.RollCount, args.Delta, args.Location);
				break;
			}
			case Event::OnMouseClick: {
				OnMouseClick(args.Button, args.Location);
				break;
			}
			case Event::OnMouseEnter: {
				OnMouseEnter(args.Location);
				this->State = ControlState::Hover;
				Invalidate();
				break;
			}
			case Event::OnMouseMove: {
				OnMouseMove(args.Location);
				break;
			}
			case Event::OnMouseDoubleClick: {
				OnMouseDoubleClick(args.Button, args.Location);
				break;
			}
			case Event::OnMouseDown: {

				OnMouseDown(args.Button, args.Location);
				this->State = ControlState::Active;
				if (ActiveStyle.Cursor) {
					::SetCursor(ActiveStyle.Cursor);
				}
				Invalidate();
				break;
			}
			case Event::OnMouseUp: {
				OnMouseUp(args.Button, args.Location);
				Invalidate();
				break;
			}
			case Event::OnMouseLeave: {
				OnMouseLeave();
				this->State = ControlState::Static;
				Invalidate();
				break;
			}
			default:
				break;
			}

		} while (false);
		if (!isRemove) {
			if (this->EventHandler) {
				this->EventHandler(this, args);
			}
			if (!isRemove) {
				this->_isRemove = NULL;
				return true;
			};
		}
		return false;
	}
	void Control::DoPaint(PaintEventArgs& args) {
		this->PublicData = args.PublicData;

		if (!_visible) { return; }//����ؼ�����Ϊ���ɼ�ֱ�Ӳ�����

		if (this->IsPendLayout()) {//���Ƶ�ʱ�����ʱ���й���Ĳ��� ����� �����ò�����Ч�����ò��ֱ�־
			this->ResumeLayout();
		}

		auto clientRect = this->GetClientRect();//��ȡ���ڴ��ڵ�λ��
		if (clientRect.IsEmptyArea()) { return; }
		auto& invalidRect = args.InvalidRectangle;
		auto& pt = args.Graphics;
		Rect _ClipRect = clientRect;
		this->ComputeClipRect();//���¼�����ڸ��׵Ĳü�����
		if (!Rect::Intersect(_ClipRect, this->ClipRect, invalidRect)) {//���ػ�������вü�
			return;
		}

		//��������+1
		args.PublicData->PaintCount++;

		_paintMtx.lock();
		this->_lastDrawRect = _ClipRect;//��¼���һ�λ��Ƶ�����
		_paintMtx.unlock();

		//���û���ƫ��
		pt.SetTransform(clientRect.X, clientRect.Y);

		args.OffSetPoint.push_back(Point(clientRect.X, clientRect.Y));

		//border��Ϣ
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
#if USED_Direct2D
		if (hasRadius) {
			//����Բ�ǿؼ� ʹ������ķ�ʽ (��������Ϊ�˿ؼ��ڲ�������ô���ƶ����ᳬ��Բ�ǲ���) �������
			Geometry roundRect(Rect(0, 0, clientRect.Width, clientRect.Height), border.TopLeftRadius, border.TopRightRadius, border.BottomRightRadius, border.BottomLeftRadius);
			Geometry _clientRect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height);
			Geometry outClipRect;
			Geometry::Intersect(outClipRect, roundRect, _clientRect);
			pt.PushLayer(outClipRect);
		}
		else {
			//��Ծ��οؼ�
			pt.PushLayer(Rect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height));
		}
#endif 
		//��ʼ����
		this->OnPaint(args);//���ƻ���������
		//�����ӿؼ�
		this->OnChildPaint(args);
		//���ƹ�����
		EzUI::ScrollBar* scrollbar = NULL;
		if (scrollbar = this->GetScrollBar()) {
			scrollbar->PublicData = args.PublicData;
			Rect barRect = scrollbar->GetClientRect();
			//����ƫ��
			pt.SetTransform(barRect.X, barRect.Y);
			scrollbar->DoPaint(args);
		}
		//����ƫ��
		pt.SetTransform(clientRect.X, clientRect.Y);
		//���Ʊ߿�
		border.Color = GetBorderColor();
		this->OnBorderPaint(args, border);//���Ʊ߿�
		args.OffSetPoint.pop_back();
		pt.PopLayer();//����
#ifdef DEBUGPAINT
		if (PublicData->Debug) {
			pt.DrawRectangle(Rect(0, 0, clientRect.Width, clientRect.Height));
	}
#endif
}

	Control::~Control()
	{
		if (PublicData) {
			PublicData->RemoveControl(this);
			PublicData = NULL;
		}
		//���ٿؼ�ǰ���Ƚ��ؼ��Ӹ��������Ƴ�
		if (Parent) {
			Parent->RemoveControl(this);
		}
		DestroySpacers();
		Parent = NULL;
	}
	void Control::DestroySpacers() {
		//�ؼ��ͷŵ�ʱ���Զ��ͷŵ���
		Controls temp = _controls;
		for (auto& it : temp) {
			if (dynamic_cast<Spacer*>(it)) {
				this->RemoveControl(it);
				delete it;
			}
		}
	}

	size_t Control::Index()
	{
		const Controls& pControls = Parent->GetControls();
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
		this->TryPendLayout();//��ӿؼ���Ҫ���������¹���
	}
	void Control::InsertControl(size_t pos, Control* ctl)
	{
		size_t i = 0;
		ControlIterator itor;
		for (auto it = _controls.begin(); it != _controls.end(); it++) {
			if (i == pos) {
				itor = it;
				break;
			}
			i++;
		}
		_controls.insert(itor, ctl);
		ctl->PublicData = this->PublicData;
		ctl->Parent = this;
		this->TryPendLayout();//��ӿؼ���Ҫ���������¹���
	}
	void Control::SetParent(Control* parentCtl)
	{
		parentCtl->AddControl(this);
	}
	void Control::RemoveControl(Control* ctl)
	{
		ControlIterator it1 = ::std::find(_controls.begin(), _controls.end(), ctl);
		if (it1 != _controls.end()) {
			ctl->OnRemove();
			this->TryPendLayout();//�Ƴ��ؼ���Ҫ���������¹���
			_controls.erase(it1);
			ControlIterator it2 = ::std::find(VisibleControls.begin(), VisibleControls.end(), ctl);
			if (it2 != VisibleControls.end()) {
				VisibleControls.erase(it2);
			}
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
	Control* Control::FindControl(const EString& objectName)
	{
		if (objectName.empty()) {
			return NULL;
		}
		if (this->Name == objectName) {
			return this;
		}
		for (auto& it : (this->_controls))
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
		std::list<Control*> ctls;
		if (attr.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->_controls))
		{
			if (it->GetAttribute(attr) == attrValue) {
				ctls.push_back(it);
			}
			auto _ctls = it->FindControl(attr, attrValue);
			for (auto& it2 : _ctls) {
				ctls.push_back(it2);
			}
		}
		return ctls;
	}
	bool Control::SwapControl(Control* ct1, Control* ct2)
	{
		int swapCount = 0;
		for (auto& it : this->_controls) {
			if (swapCount == 2) {
				break;
			}
			if (it == ct1) {
				it = ct2;
				swapCount++;
				continue;
			}
			if (it == ct2) {
				it = ct1;
				swapCount++;
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

	bool Control::IsInWindow() {
		bool visible = this->IsVisible();
		Control* pCtl = this->Parent;
		while (pCtl && visible)
		{
			visible = pCtl->IsVisible();
			pCtl = pCtl->Parent;
		}
		return visible;
	}

	bool Control::Invalidate() {
		std::unique_lock<std::mutex> autoLock(_paintMtx);
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
	Rect Control::GetCareRect()
	{
		return Rect();
	}
	bool Control::IsAutoWidth()
	{
		return false;
	}
	bool Control::IsAutoHeight()
	{
		return false;
	}
	void Control::SetAutoWidth(bool flag)
	{
	}
	void Control::SetAutoHeight(bool flag)
	{
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);//���ø��ؼ��Ĳü�����
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->ClipRect);//����͸��ؼ��ԱȽϲü�����
		}
		else {
			Rect& ClipRectRef = *(Rect*)(&this->ClipRect);
			ClipRectRef = this->GetClientRect();
		}
	}
	const Controls& Control::GetControls()
	{
		return _controls;
	}
	Control* Control::FindControl(size_t pos)
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
	void Control::Clear(bool freeControls)
	{
		Controls temp = _controls;
		for (auto i = temp.begin(); i != temp.end(); i++)
		{
			(*i)->OnRemove();
			if (freeControls) {
				delete* i;
			}
		}
		VisibleControls.clear();//��տɼ��ؼ�
		_controls.clear();//����ӿؼ�����
		DestroySpacers();//��յ��ɲ�ɾ������
	}
	void Control::OnKeyChar(WPARAM wParam, LPARAM lParam) {
	}
	void Control::OnKeyDown(WPARAM wParam, LPARAM lParam) {
	}
	void Control::OnKeyUp(WPARAM wParam, LPARAM lParam) {
	}
	void Control::OnMouseMove(const Point& point)
	{
	}
	void Control::OnMouseWheel(int _rollCount, short zDelta, const Point& point)
	{
	}
	void Control::OnMouseClick(MouseButton mbtn, const Point& point)
	{
	}
	void Control::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
	{
	}
	void Control::OnMouseEnter(const Point& point)
	{
	}
	void Control::OnMouseDown(MouseButton mbtn, const Point& point)
	{
	}
	void Control::OnMouseUp(MouseButton mbtn, const Point& point)
	{
	}
	void Control::OnMouseLeave()
	{
	}
	void Control::OnLocation(const Point& pt)
	{
	}
	void Control::OnSize(const Size& size)
	{
	}
	void Control::OnRect(const Rect& rect)
	{
	}
	void Control::OnKillFocus(Control* control)
	{
	}
	ScrollBar* Control::GetScrollBar()
	{
		return NULL;
	}
	bool Control::DispatchEvent(const MouseEventArgs& arg)
	{
		return this->OnMouseEvent(arg);
	}
	bool Control::DispatchEvent(const KeyboardEventArgs& arg)
	{
		return this->OnKeyBoardEvent(arg);
	}
	bool Control::DispatchEvent(const KillFocusEventArgs& arg)
	{
		this->OnKillFocus(arg.Control);
		return false;
	}
};