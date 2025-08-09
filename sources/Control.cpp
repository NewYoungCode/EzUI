#include "Control.h"
#include "ScrollBar.h"
#include "Spacer.h"

namespace ezui {
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
		return std::abs(value) > 1e-6f;
	}
	inline bool __IsValid(const Image* value) {
		return value != NULL;
	}
	inline bool __IsValid(const Color& value) {
		return value.GetValue() != 0;
	}
	inline bool __IsValid(const UIString& value) {
		return !value.empty();
	}
	inline bool __IsValid(const std::wstring& value) {
		return !value.empty();
	}
	inline bool __IsValid(StrokeStyle value) {
		return value != StrokeStyle::None;
	}

	//是否是像素单位
	inline bool __IsPx(const UIString& num, float& outNum) {
		size_t pos = num.find("px");
		if (pos != 0 && pos == num.size() - 2) {
			outNum = std::stof(num.substr(0, pos).c_str());
			return true;
		}
		return false;
	}
	//字符串转数值
	inline float __ToFloat(const UIString& numStr) {
		float value;
		if (__IsPx(numStr, value)) {
			return value;
		}
		return 0;//解析失败
	}

	//处理url(d:/imgs/aa.png)路径
	inline Image* __MakeImage(UIString value) {
		value = value.replace("\"", "");//删除双引号;
		auto pos1 = value.find("(");
		auto pos2 = value.find(")");
		if (pos1 != size_t(-1) && pos2 != size_t(-1)) {
			//background-image:url(res/images/xxx.png)的方式
			value = value.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		return Image::Make(value);
	}

	//构建边框信息并回调
	inline void __MakeBorder(const UIString str, Border& bd, const std::function<void(float)>& callback) {
		auto values = str.split(" ");
		for (auto& v : values) {
			float num;
			if (v == "dashed") {
				bd.Style = StrokeStyle::Dash;
				continue;
			}
			else if (v == "solid") {
				bd.Style = StrokeStyle::Solid;
				continue;
			}
			else if (v == "none") {
				bd.Style = StrokeStyle::None;
				continue;
			}
			else if (__IsPx(v, num)) {
				callback(num);
				continue;
			}
			else
			{
				bool isGood = false;
				Color color = Color::Make(v, &isGood);
				if (isGood) {
					bd.Color = color;
				}
			}
		}
	}
};
namespace ezui {

#define UI_SUPER_STYLE_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
		if (_state == ControlState::None) { \
			_state = this->State; \
		}\
		##_type _##_filed;\
		if ((_state & ControlState::Checked) == ControlState::Checked) {\
			_##_filed = this->GetStyle(ControlState::Checked).##_filed;\
			if (__IsValid(_##_filed)) {\
				return _##_filed;\
			}\
		}\
		if ((_state & ControlState::Hover) == ControlState::Hover) {\
			_##_filed = this->GetStyle(ControlState::Hover).##_filed;\
			if (__IsValid(_##_filed)) {\
				return _##_filed;\
			}\
		}\
		if ((_state & ControlState::Active) == ControlState::Active) {\
			_##_filed = this->GetStyle(ControlState::Active).##_filed;\
			if (__IsValid(_##_filed)) {\
				return _##_filed;\
			}\
		}\
		_##_filed = this->GetStyle(ControlState::Static).##_filed; \
		if (__IsValid(_##_filed)) {\
				return _##_filed; \
		}\
		Control* parentCtrl = this->Parent; \
		while (parentCtrl){ \
			_##_filed = parentCtrl->GetStyle(ControlState::Static).##_filed; \
			if (__IsValid(_##_filed)) {\
				break; \
			}\
			parentCtrl = parentCtrl->Parent; \
		}\
		return _##_filed; \
	}\

#define UI_STYLE_BINDFUNC(_type,_filed)  _type Control:: ##Get ##_filed(ControlState _state)  { \
		if (_state == ControlState::None) {\
			_state = this->State;\
		}\
		##_type _##_filed;\
		if ((_state & ControlState::Checked) == ControlState::Checked) {\
			_##_filed = this->GetStyle(ControlState::Checked).##_filed;\
			if (__IsValid(_##_filed))return _##_filed;\
		}\
		if ((_state & ControlState::Hover) == ControlState::Hover) {\
			_##_filed = this->GetStyle(ControlState::Hover).##_filed;\
			if (__IsValid(_##_filed))return _##_filed;\
		}\
		if ((_state & ControlState::Active) == ControlState::Active) {\
			_##_filed = this->GetStyle(ControlState::Active).##_filed;\
			if (__IsValid(_##_filed))return _##_filed;\
		}\
		return this->GetStyle(ControlState::Static).##_filed;;\
}\

#define UI_BORDER_BINDFUNC(_type,_filed1,_filed)  _type Control:: ##Get ##_filed1 ##_filed(ControlState _state)  { \
		if (_state == ControlState::None) {\
			_state = this->State;\
		}\
		##_type _##_filed;\
		if ((_state & ControlState::Checked) == ControlState::Checked) {\
			_##_filed = this->GetStyle(ControlState::Checked).##_filed1.##_filed;\
			if (__IsValid(_##_filed)) return _##_filed;\
		}\
		if ((_state & ControlState::Hover) == ControlState::Hover) {\
			_##_filed = this->GetStyle(ControlState::Hover).##_filed1.##_filed;\
			if (__IsValid(_##_filed)) return _##_filed;\
		}\
		if ((_state & ControlState::Active) == ControlState::Active) {\
			_##_filed = this->GetStyle(ControlState::Active).##_filed1.##_filed;\
			if (__IsValid(_##_filed)) return _##_filed;\
		}\
		return this->GetStyle(ControlState::Static).##_filed1.##_filed; \
}\


	UI_BORDER_BINDFUNC(int_t, Border, TopLeftRadius);
	UI_BORDER_BINDFUNC(int_t, Border, TopRightRadius);
	UI_BORDER_BINDFUNC(int_t, Border, BottomRightRadius);
	UI_BORDER_BINDFUNC(int_t, Border, BottomLeftRadius);
	UI_BORDER_BINDFUNC(int_t, Border, Left);
	UI_BORDER_BINDFUNC(int_t, Border, Top);
	UI_BORDER_BINDFUNC(int_t, Border, Right);
	UI_BORDER_BINDFUNC(int_t, Border, Bottom);
	UI_BORDER_BINDFUNC(Color, Border, Color);
	UI_BORDER_BINDFUNC(StrokeStyle, Border, Style);

	UI_STYLE_BINDFUNC(Color, BackColor);
	UI_STYLE_BINDFUNC(Image*, ForeImage);
	UI_STYLE_BINDFUNC(Image*, BackImage);
	UI_STYLE_BINDFUNC(float, Angle);

	UI_SUPER_STYLE_BINDFUNC(int_t, FontSize);
	UI_SUPER_STYLE_BINDFUNC(Color, ForeColor);
	UI_SUPER_STYLE_BINDFUNC(std::wstring, FontFamily);

	WindowData* Control::GetPublicData()
	{
		return (WindowData*)UI_GET_USERDATA(Hwnd());
	}

	Control::Control(Object* parentObject) :Object(parentObject)
	{
	}
	void Control::OnChildPaint(PaintEventArgs& args)
	{
		ViewControls.clear();
		//绘制子控件
		Rect rect(0, 0, Width(), Height());
		for (auto& it : m_controls) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			it->SendEvent(args);
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
			e.Graphics.FillRectangle(RectF{ 0,0,(float)m_rect.Width,(float)m_rect.Height });
		}
		if (backgroundImage != NULL) {
			e.Graphics.DrawImage(backgroundImage, RectF{ 0,0,(float)m_rect.Width,(float)m_rect.Height });
		}
	}
	void Control::OnForePaint(PaintEventArgs& e) {
		Image* foreImage = this->GetForeImage();
		if (foreImage) {
			e.Graphics.DrawImage(foreImage, RectF{ 0,0,(float)m_rect.Width,(float)m_rect.Height });
		}
	}
	void Control::OnBorderPaint(PaintEventArgs& e, const Border& border)
	{
		const Color& borderColor = border.Color;
		if (borderColor.GetValue() == 0) return;//边框无效颜色不绘制
		if (border.Style == StrokeStyle::None)return;//未指定边框风格 不允许绘制
		int_t borderLeft = border.Left;
		int_t borderTop = border.Top;
		int_t borderRight = border.Right;
		int_t borderBottom = border.Bottom;
		int_t topLeftRadius = border.TopLeftRadius;
		int_t topRightRadius = border.TopRightRadius;
		int_t bottomRightRadius = border.BottomRightRadius;
		int_t bottomLeftRadius = border.BottomLeftRadius;

		//指定边框风格
		e.Graphics.SetStrokeStyle(border.Style);
		//规则的矩形
		if (topLeftRadius == 0 && topRightRadius == 0 && bottomLeftRadius == 0 && bottomRightRadius == 0) {
			bool hasBorder = borderLeft || borderTop || borderRight || borderBottom;
			if (!hasBorder) return;//边框为0不绘制
			e.Graphics.SetColor(borderColor);
			if (borderLeft > 0) {
				float half = borderLeft / 2.0f;
				e.Graphics.DrawLine(PointF{ half, 0 }, PointF{ half, (float)Height() }, borderLeft);
			}
			if (borderTop > 0) {
				float half = borderTop / 2.0f;
				e.Graphics.DrawLine(PointF{ 0, half }, PointF{ (float)Width(), half }, borderTop);
			}
			if (borderRight > 0) {
				float half = borderRight / 2.0f;
				e.Graphics.DrawLine(PointF{ (float)Width() - half, 0 }, PointF{ (float)Width() - half, (float)Height() }, borderRight);
			}
			if (borderBottom > 0) {
				float half = borderBottom / 2.0f;
				e.Graphics.DrawLine(PointF{ 0, (float)Height() - half }, PointF{ (float)Width(), (float)Height() - half }, borderBottom);
			}
		}
		else {
			int_t value1 = borderLeft > borderTop ? borderLeft : borderTop;
			int_t value2 = borderRight > borderBottom ? borderRight : borderBottom;
			int_t maxBorder = value1 > value2 ? value1 : value2;
			if (maxBorder <= 0) return;
			float half = maxBorder / 2.0f;
			e.Graphics.SetColor(borderColor);

#define max(a,b)  (((a) > (b)) ? (a) : (b))
			// 减去半个边框宽度，确保所有边线和圆角都在控件内
			auto shrink = [&](int r) -> float {
				return max(0.0f, (float)r - half);
				};
			RectangleGeometry rr(
				RectF(half, half, Width() - half * 2.0f, Height() - half * 2.0f),
				shrink(topLeftRadius),
				shrink(topRightRadius),
				shrink(bottomRightRadius),
				shrink(bottomLeftRadius)
			);
			e.Graphics.DrawGeometry(rr.Get(), maxBorder);
		}
	}
	ControlStyle& Control::GetStyle(const ControlState& _state) {
		if ((_state & ControlState::Hover) == ControlState::Hover) {
			return this->HoverStyle;
		}
		else if ((_state & ControlState::Active) == ControlState::Active) {
			return this->ActiveStyle;
		}
		return this->Style;
	}

	void Control::SetAttribute(const UIString& attrName, const UIString& attrValue)
	{
		__super::SetAttribute(attrName, attrValue);
		do
		{
			if (attrName == "name" || attrName == "id") {
				this->Name = attrValue;
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
			if (attrName == "location") {
				auto arr = attrValue.split(",");
				int_t x = std::stoi(arr[0]);
				int_t y = std::stoi(arr[1]);
				this->SetLocation(Point(x, y));
				break;
			}
			if (attrName == "size") {
				if (attrValue == "auto") {
					this->SetAutoWidth(true);
					this->SetAutoHeight(true);
				}
				else {
					auto arr = attrValue.split(",");
					int_t width = std::stoi(arr[0]);
					int_t height = std::stoi(arr[1]);
					this->SetFixedSize(Size(width, height));
				}
				break;
			}
			if (attrName == "width") {
				if (attrValue == "auto") {
					this->SetAutoWidth(true);//自动宽度
				}
				else {
					if (attrValue.count(".") > 0) {
						this->SetRateWidth(std::stof(attrValue.c_str()));//基于父控件的百分比
					}
					else {
						this->SetFixedWidth(std::stoi(attrValue));//如果单独设置了宽高那就是绝对宽高了
					}
				}
				break;
			}
			if (attrName == "height") {
				if (attrValue == "auto") {
					this->SetAutoHeight(true);//自动高度
				}
				else {
					if (attrValue.count(".") > 0) {
						this->SetRateHeight(std::stof(attrValue.c_str()));//基于父控件的百分比
					}
					else {
						this->SetFixedHeight(std::stoi(attrValue));//如果单独设置了宽高那就是绝对宽高了
					}
				}
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
			if (attrName == "visible") {
				this->m_bVisible = (::strcmp(attrValue.c_str(), "false") == 0 ? false : true);
				break;
			}
			if (attrName == "tips") {
				this->SetTips(attrValue);
				break;
			}
			if (attrName == "dock") {
				if (attrValue == "fill") {
					this->m_dock = DockStyle::Fill; break;
				}
				if (attrValue == "horizontal") {
					this->m_dock = DockStyle::Horizontal; break;
				}
				if (attrValue == "vertical") {
					this->m_dock = DockStyle::Vertical; break;
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
		return this->m_scale;
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
		return m_rect.X;
	}
	int_t Control::Y()
	{
		return m_rect.Y;
	}
	int_t Control::Width()
	{
		return m_rect.Width;
	}
	int_t Control::Height()
	{
		return m_rect.Height;
	}
	void Control::SetX(int_t X) {
		SetLocation({ X,m_rect.Y });
	}
	void Control::SetY(int_t Y) {
		SetLocation({ m_rect.X,Y });
	}
	void Control::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, m_rect.Width, m_rect.Height));
	}
	void Control::SetWidth(int_t width) {
		SetSize({ width,m_rect.Height });
	}
	void Control::SetHeight(int_t height) {
		SetSize({ m_rect.Width,height });
	}
	void Control::SetSize(const Size& size)
	{
		SetRect({ m_rect.X,m_rect.Y,size.Width,size.Height });
	}
	Point Control::GetLocation()
	{
		return m_rect.GetLocation();
	}
	Size Control::GetSize()
	{
		return m_rect.GetSize();
	}
	const Rect& Control::GetRect()
	{
		return m_rect;
	}
	void Control::SetFixedWidth(int_t fixedWidth)
	{
		m_fixedSize.Width = fixedWidth;
		SetRect({ m_rect.X,m_rect.Y,fixedWidth,m_rect.Height });
	}
	void Control::SetFixedHeight(int_t fixedHeight)
	{
		m_fixedSize.Height = fixedHeight;
		SetRect({ m_rect.X,m_rect.Y,m_rect.Width,fixedHeight });
	}
	void Control::SetRateWidth(float rateWidth)
	{
		m_rateSize.Width = rateWidth;
	}
	void Control::SetRateHeight(float rateHeight)
	{
		m_rateSize.Height = rateHeight;
	}
	void Control::SetFixedSize(const Size& size)
	{
		m_fixedSize.Width = size.Width;
		m_fixedSize.Height = size.Height;
		SetRect({ m_rect.X,m_rect.Y,size.Width,size.Height });
	}
	int_t Control::GetFixedWidth()
	{
		if (Parent) {
			if (m_rateSize.Width > 1e-6f) {
				m_fixedSize.Width = Parent->Width() * m_rateSize.Width + 0.5;
			}
		}
		return m_fixedSize.Width;
	}
	int_t Control::GetFixedHeight()
	{
		if (Parent) {
			if (m_rateSize.Height > 1e-6f) {
				m_fixedSize.Height = Parent->Height() * m_rateSize.Height + 0.5;
			}
		}
		return m_fixedSize.Height;
	}
	bool Control::IsPendLayout() {
		return this->m_layoutState == LayoutState::Pend;
	}
	const LayoutState Control::TryPendLayout() {
		if (this->m_layoutState == LayoutState::None) {
			this->m_layoutState = LayoutState::Pend;
		}
		return this->m_layoutState;
	}
	const LayoutState Control::GetLayoutState()
	{
		return this->m_layoutState;
	}
	void Control::EndLayout() {
		this->m_layoutState = LayoutState::None;
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
	Rect Control::GetScreenRect()
	{
		RECT winRect{ 0,0,0,0 };
		GetWindowRect(Hwnd(), &winRect);//获取外层窗口基于屏幕的坐标
		Rect ctrlRect = GetClientRect();//获取当前控件基于客户端窗口的坐标
		ctrlRect.X += winRect.left;
		ctrlRect.Y += winRect.top;
		return ctrlRect;
	}
	DockStyle Control::GetDockStyle()
	{
		return this->m_dock;
	}
	void Control::SetDockStyle(const DockStyle& dockStyle)
	{
		if (dockStyle != this->m_dock && Parent) {
			Parent->TryPendLayout();
		}
		this->m_dock = dockStyle;
	}
	const Rect& Control::SetRect(const Rect& rect)
	{
		this->m_rect = rect;

		if (GetFixedWidth() > 0) {
			m_rect.Width = GetFixedWidth();
		}
		if (GetFixedHeight() > 0) {
			m_rect.Height = GetFixedHeight();
		}

		Point newLocation = m_rect.GetLocation();
		Size newSize = m_rect.GetSize();

		if (!m_lastLocation.Equals(newLocation)) {
			this->SendEvent(MoveEventArgs(newLocation));
			m_lastLocation = newLocation;
		}
		if (!newSize.Equals(m_lastSize)) {
			this->SendEvent(SizeEventArgs(newSize));
			m_lastSize = newSize;
		}
		return this->m_rect;
	}
	void Control::SetTips(const UIString& text)
	{
		m_tipsText = text;
	}
	const UIString& Control::GetTips()
	{
		return m_tipsText;
	}
	void Control::RefreshLayout()
	{
		if (this->m_layoutState == LayoutState::Layouting) {
			return;
		}
		this->m_layoutState = LayoutState::Layouting;//布局中
		if (GetScrollBar()) {//如果存在滚动条就设置滚动条的矩形位置
			GetScrollBar()->ParentSize({ m_rect.Width,m_rect.Height });
		}
		m_contentSize.Width = 0;
		m_contentSize.Height = 0;
		this->OnLayout();
		if (IsAutoHeight() && Height() != m_contentSize.Height) {
			this->SetFixedHeight(m_contentSize.Height);
			this->EndLayout();
			this->RefreshLayout();
			if (Parent) {
				Parent->RefreshLayout();
				Parent->Invalidate();
			}
			return;
		}
		if (IsAutoWidth() && Width() != m_contentSize.Width) {
			this->SetFixedWidth(m_contentSize.Width);
			this->EndLayout();
			this->RefreshLayout();
			if (Parent) {
				Parent->RefreshLayout();
				Parent->Invalidate();
			}
			return;
		}
		this->EndLayout();
	}
	void Control::OnLayout() {
		m_contentSize.Width = 0;
		m_contentSize.Height = 0;
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
			if (_width > m_contentSize.Width) {
				m_contentSize.Width = _width;
			}
			_height = it->Y() + it->Height();
			if (_height > m_contentSize.Height) {
				m_contentSize.Height = _height;
			}
		}
		this->EndLayout();
	}
	bool Control::SendEvent(const EventArgs& arg)
	{
		return this->OnEvent((EventArgs&)arg);
	}
	bool Control::OnEvent(EventArgs& arg)
	{
		bool isRemove = false;
		this->m_bRemove = &isRemove;
		do
		{
			if (arg.EventType == Event::OnPaint && !IsVisible()) {
				break;
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
		do
		{
			auto* publicData = GetPublicData();
			if (publicData && ((this->EventFilter & arg.EventType) == arg.EventType)) {
				if (arg.EventType != Event::OnPaint) {
					bool bHandle = publicData->SendNotify(this, (EventArgs&)arg);
					if (bHandle) {
						//如果处理过了则不需要继续往下派发
						break;
					}
					if (isRemove) {
						break;
					}
				}
			}
		} while (false);
		if (!isRemove) {
			//通用事件处理 ps:绘制函数比较特殊(在其他地方处理)
			if (this->EventHandler && (arg.EventType != Event::OnPaint)) {
				this->EventHandler(this, arg);
			}
			if (!isRemove) {
				this->m_bRemove = NULL;
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
				this->Parent->SendEvent(copy_args);//如果设置了穿透就直接发送给上一层控件
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
			this->Parent->SendEvent(copy_args);//如果设置了穿透就发送给上一层控件
		}
	}
	void Control::OnPaintBefore(PaintEventArgs& args) {
		this->SetHwnd(args.HWND);
		if (this->IsPendLayout()) {//绘制的时候会检查时候有挂起的布局 如果有 立即让布局生效并重置布局标志
			this->RefreshLayout();
		}
		auto clientRect = this->GetClientRect();//获取基于窗口的位置
		if (clientRect.IsEmptyArea()) { return; }
		auto& invalidRect = args.InvalidRectangle;
		auto& pt = args.Graphics;
		Rect _ClipRect = clientRect;
		this->ComputeClipRect();//重新计算基于父亲的裁剪区域
		if (!Rect::Intersect(_ClipRect, this->m_viewRect, invalidRect)) {//和重绘区域进行裁剪
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
			RectangleGeometry roundRect(Rect(0, 0, clientRect.Width, clientRect.Height), border.TopLeftRadius, border.TopRightRadius, border.BottomRightRadius, border.BottomLeftRadius);
			RectangleGeometry _clientRect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height);
			Geometry outClipRect;
			Geometry::Intersect(outClipRect, roundRect, _clientRect);
			args.PushLayer(outClipRect);
		}
		else {
			//针对矩形控件
			args.PushLayer(Rect(_ClipRect.X - clientRect.X, _ClipRect.Y - clientRect.Y, _ClipRect.Width, _ClipRect.Height));
		}
#endif 
		auto* publicData = GetPublicData();
		//调用公共函数,如果那边不拦截,就开始绘制自身基本上下文
		bool bHandle = publicData->SendNotify(this, args);
		if (!bHandle) {
			this->OnPaint(args);
		}

		//绘制子控件
		this->OnChildPaint(args);
		//绘制滚动条
		ScrollBar* scrollbar = NULL;
		if (scrollbar = this->GetScrollBar()) {
			scrollbar->SendEvent(args);
		}
		//绘制边框
		border.Color = GetBorderColor();
		border.Style = GetBorderStyle();
		this->OnBorderPaint(args, border);//绘制边框
#ifdef _DEBUG
		if (publicData->Debug) {
			float width = 1 * this->GetScale();
			pt.SetColor(publicData->DebugColor);
			pt.DrawRectangle(RectF(0, 0, clientRect.Width, clientRect.Height), 0, width);
		}
#endif
		if (this->EventHandler) {
			this->EventHandler(this, args);
		}
		args.PopLayer();//弹出纹理层
		args.PopOffset();//弹出偏移
		if (angle != 0) {
			pt.SetTransform(0, 0, 0);
		}
	}
	void Control::OnDpiChange(const DpiChangeEventArgs& arg)
	{
		float scale = arg.Scale / this->m_scale;
		this->m_scale = arg.Scale;

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
			it->SendEvent(arg);
		}

		if (needScale && this->GetScrollBar()) {
			this->GetScrollBar()->SendEvent(arg);
		}

	}
	Control::~Control()
	{
		auto* publicData = GetPublicData();
		//清除绑定信息
		if (publicData) {
			publicData->CleanControl(this);
		}
		//释放弹簧
		DestroySpacers();
	}
	void Control::DestroySpacers() {
		for (auto itor = m_spacers.begin(); itor != m_spacers.end(); /* no increment */)
		{
			this->Remove(*itor, true);  // 移除弹簧控件 并且释放控件 
			itor = m_spacers.begin();   // 获取新的迭代器
		}
	}

	Control* Control::Attach(Control* ctrl)
	{
		__super::Attach(ctrl);
		return ctrl;
	}
	void Control::Detach(Control* ctrl)
	{
		__super::Detach(ctrl);
	}

	Image* Control::Attach(Image* img)
	{
		m_imgs.Add(img);
		return img;
	}
	void Control::Detach(Image* img)
	{
		m_imgs.Remove(img);
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
		auto itor = std::find(m_controls.begin(), m_controls.end(), ctl);
		if (itor != m_controls.end()) {
			ASSERT(!"The control already exists and cannot be added repeatedly");
		}
#endif
		if (dynamic_cast<Spacer*>(ctl)) {
			m_spacers.push_back(ctl);
		}
		m_controls.push_back(ctl);
		ctl->SetHwnd(this->Hwnd());
		ctl->Parent = this;

		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendEvent(DpiChangeEventArgs(this->GetScale()));
		}

		ctl->TryPendLayout();
		this->TryPendLayout();//添加控件需要将布局重新挂起
		return ctl;
	}
	void Control::Insert(int_t pos, Control* ctl)
	{
#ifdef _DEBUG
		{
			auto itor = std::find(m_controls.begin(), m_controls.end(), ctl);
			if (itor != m_controls.end()) {
				ASSERT(!"The control already exists and cannot be added repeatedly");
			}
		}
#endif
		if (dynamic_cast<Spacer*>(ctl)) {
			m_spacers.push_back(ctl);
		}
		size_t i = 0;
		auto itor = m_controls.begin();
		for (; itor != m_controls.end(); ++itor) {
			if (i == pos) {
				break;
			}
			++i;
		}
		if (itor == m_controls.end()) {
			m_controls.push_back(ctl);
		}
		else {
			m_controls.insert(itor, ctl);
		}
		ctl->SetHwnd(this->Hwnd());
		ctl->Parent = this;
		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendEvent(DpiChangeEventArgs(this->GetScale()));
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
		auto itor = ::std::find(m_controls.begin(), m_controls.end(), ctl);
		if (itor != m_controls.end()) {
			ctl->OnRemove();
			this->TryPendLayout();//移除控件需要将布局重新挂起
			m_controls.erase(itor);
			auto itor2 = ::std::find(ViewControls.begin(), ViewControls.end(), ctl);
			if (itor2 != ViewControls.end()) {
				ViewControls.erase(itor2);
			}
			if (freeCtrl) {
				delete ctl;
			}
		}
		//子对象存在相同的对象也要跟随移除
		this->Detach(ctl);
		//如果是弹簧控件 顺便把弹簧容器中的item也移除
		auto itorSpacer = ::std::find(m_spacers.begin(), m_spacers.end(), ctl);
		if (itorSpacer != m_spacers.end()) {
			m_spacers.erase(itorSpacer);
		}
	}
	void Control::OnRemove()
	{
		if (m_bRemove) {
			*m_bRemove = true;
		}
		for (auto& it : m_controls) {
			it->OnRemove();
		}
		auto* publicData = GetPublicData();
		if (publicData) {
			publicData->CleanControl(this);
		}
	}
	Control* Control::FindControl(const UIString& ctlName)
	{
		if (ctlName.empty()) {
			return NULL;
		}
		if (this->Name == ctlName) {
			return this;
		}
		for (auto& it : (this->m_controls))
		{
			if (it->Name == ctlName) {
				return it;
			}
			auto ctl = it->FindControl(ctlName);
			if (ctl) return ctl;
		}
		return NULL;
	}
	Controls Control::FindControl(const UIString& attrName, const UIString& attrValue)
	{
		Controls ctls;
		if (attrName.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->m_controls))
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
	Control* Control::FindSingleControl(const UIString& attrName, const UIString& attrValue) {
		auto list = this->FindControl(attrName, attrValue);
		if (list.size() > 0) {
			return *list.begin();
		}
		return NULL;
	}

	Control* Control::FindChild(const UIString& ctlName)
	{
		if (ctlName.empty()) {
			return NULL;
		}
		for (auto& it : (this->m_controls))
		{
			if (it->Name == ctlName) {
				return it;
			}
		}
		return NULL;
	}
	Controls Control::FindChild(const UIString& attrName, const UIString& attrValue)
	{
		Controls ctls;
		if (attrName.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->m_controls))
		{
			if (it->GetAttribute(attrName) == attrValue) {
				ctls.push_back(it);
			}
		}
		return ctls;
	}
	Control* Control::FindSingleChild(const UIString& ctlName, const UIString& attrValue) {
		auto list = this->FindChild(ctlName, attrValue);
		if (list.size() > 0) {
			return *list.begin();
		}
		return NULL;
	}

	bool Control::SwapChild(Control* ct1, Control* ct2)
	{
		int_t swapCount = 0;
		for (auto& it : this->m_controls) {
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
		if (flag != this->m_bVisible && this->Parent) {
			this->Parent->TryPendLayout();
		}
		this->m_bVisible = flag;
	}
	bool Control::IsVisible() {
		return this->m_bVisible;
	}

	void Control::Hide() {
		m_bVisible = false;
		this->Invalidate();
	}

	void Control::Show() {
		m_bVisible = true;
		this->Invalidate();
	}

	bool Control::Invalidate() {
		auto* publicData = GetPublicData();
		if (publicData) {
			if (Parent && this->IsPendLayout()) {
				return Parent->Invalidate();
			}
			float angle = this->GetAngle();
			if (!(angle == 0 || angle == 180) && Parent) {
				return Parent->Invalidate();
			}
			if (publicData) {
				Rect _InvalidateRect = GetClientRect();
				publicData->InvalidateRect(_InvalidateRect);
				return true;
			}
		}
		return false;
	}
	void Control::Refresh() {
		if (Invalidate()) {
			auto* publicData = GetPublicData();
			if (publicData) {
				publicData->UpdateWindow();//立即更新全部无效区域
			}
		}
	}
	Rect Control::GetCareRect()
	{
		return Rect();
	}
	const Size& Control::GetContentSize()
	{
		return m_contentSize;
	}
	bool Control::IsAutoWidth()
	{
		return this->m_bAutoWidth;
	}
	void Control::SetAutoSize(bool flag)
	{
		this->SetAutoWidth(flag);
		this->SetAutoHeight(flag);
	}
	void Control::SetAutoWidth(bool flag)
	{
		if (flag != this->m_bAutoWidth && Parent) {
			Parent->TryPendLayout();
		}
		if (flag && Width() == 0) {
			m_rect.Width = 1;
		}
		this->m_bAutoWidth = flag;
	}
	bool Control::IsAutoHeight()
	{
		return this->m_bAutoHeight;
	}
	void Control::SetAutoHeight(bool flag)
	{
		if (flag != this->m_bAutoHeight && Parent) {
			Parent->TryPendLayout();
		}
		if (flag && Height() == 0) {
			m_rect.Height = 1;
		}
		this->m_bAutoHeight = flag;
	}
	void Control::SetContentWidth(int_t width)
	{
		this->m_contentSize.Width = width;
	}
	void Control::SetContentHeight(int_t height)
	{
		this->m_contentSize.Height = height;
	}
	void Control::SetContentSize(const Size& size)
	{
		this->m_contentSize = size;
	}
	void Control::ComputeClipRect()
	{
		if (Parent) {
			Rect& ClipRectRef = *(Rect*)(&this->m_viewRect);//引用父控件的裁剪区域
			Rect::Intersect(ClipRectRef, this->GetClientRect(), Parent->m_viewRect);//自身和父控件对比较裁剪区域
		}
		else {
			Rect& ClipRectRef = *(Rect*)(&this->m_viewRect);
			ClipRectRef = this->GetClientRect();
		}
	}
	const Controls& Control::GetControls()
	{
		return m_controls;
	}
	Control* Control::GetControl(int_t pos)
	{
		int_t _pos = 0;
		for (auto& it : m_controls) {
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
		auto temp = m_controls;
		for (auto itor = temp.begin(); itor != temp.end(); ++itor)
		{
			Control* it = *itor;
			it->OnRemove();
			if (freeChilds) {
				//子对象存在相同的对象也要跟随移除
				this->Detach(it);
				delete it;
			}
		}
		this->ViewControls.clear();//清空可见控件
		this->m_controls.clear();//清空子控件集合
		this->m_spacers.clear();//清空弹簧
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
	void Control::OnMouseDoubleClick(const MouseEventArgs& args)
	{
	}
	void Control::OnMouseEnter(const MouseEventArgs& args)
	{
		this->State = ControlState::Hover;
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

	void Control::SetStyle(ControlStyle& style, const UIString& key, const UIString& value_)
	{
		UIString value = value_;
		do
		{
			if (key == "width") {
				if (value.count(".") > 0) {
					this->SetRateWidth(std::stof(value.c_str()));
				}
				else {
					this->SetFixedWidth(__ToFloat(value));
				}
				break;
			}
			if (key == "height") {
				if (value.count(".") > 0) {
					this->SetRateHeight(std::stof(value.c_str()));
				}
				else {
					this->SetFixedHeight(__ToFloat(value));
				}
				break;
				if (key == "pointer-events") {
					if (value == "none") {
						//忽略鼠标事件 将直接穿透
						this->EventPassThrough = this->EventPassThrough | Event::OnMouseEvent;
					}
					else if (value == "auto") {
						this->EventPassThrough = Event::None;
					}
					break;
				}
				if (key == "display") {
					this->SetVisible(value != "none");
					break;
				}
			}

			if (key == "cursor") {
				if (value == "pointer") {
					style.Cursor = LoadCursor(ezui::Cursor::HAND);
				}
				else if (value == "help") {
					style.Cursor = LoadCursor(ezui::Cursor::HELP);
				}
				else if (value == "n-resize") {
					//南北箭头 纵向
					style.Cursor = LoadCursor(ezui::Cursor::SIZENS);
				}
				else if (value == "e-resize") {
					//东西箭头 水平
					style.Cursor = LoadCursor(ezui::Cursor::SIZEWE);
				}
				else if (value == "move") {
					//四个方向的箭头都有
					style.Cursor = LoadCursor(ezui::Cursor::SIZEALL);
				}
				break;
			}
			if (key == "background-color") {
				style.BackColor = Color::Make(value);
				break;
			}
			if (key == "background-image") {
				style.BackImage = this->Attach(__MakeImage(value));
				break;
			}
			if (key == "background-position" && style.BackImage) {
				if (value.count("px") >= 2) {
					auto pxs = value.split(" ");
					float x = 0;
					__IsPx(pxs[0], x);
					style.BackImage->DrawPosition.X = x;

					float y = 0;
					__IsPx(pxs[1], y);
					style.BackImage->DrawPosition.Y = y;
				}
				break;
			}
			if (key == "background-size" || key == "background-image-size") {
				if (value == "auto" && style.BackImage) {
					style.BackImage->SizeMode = ImageSizeMode::OriginalSize;
				}
				else if (value.count("px") >= 2 && style.BackImage) {
					auto pxs = value.split(" ");
					float w = 0;
					__IsPx(pxs[0], w);
					style.BackImage->DrawSize.Width = w;

					float h = 0;
					__IsPx(pxs[1], h);
					style.BackImage->DrawSize.Height = h;
				}
				break;
			}
			if (key == "fore-image") {
				style.ForeImage = this->Attach(__MakeImage(value));
				break;
			}
			if (key == "fore-image-size") {
				if (value == "auto" && style.ForeImage) {
					style.ForeImage->SizeMode = ImageSizeMode::OriginalSize;
				}
				break;
			}
			if (key == "color" || key == "fore-color") {
				style.ForeColor = Color::Make(value);
				break;
			}
			if (key == "border-color") {
				style.Border.Color = Color::Make(value);
				break;
			}
			if (key == "border-style") {
				if (value == "solid") {
					style.Border.Style = StrokeStyle::Solid;
				}
				else if (value == "dashed") {
					style.Border.Style = StrokeStyle::Dash;
				}
				break;
			}
			if (key == "border-radius") {
				style.Border.Radius = __ToFloat(value);
				break;
			}
			if (key == "border-top-left-radius") {
				style.Border.TopLeftRadius = __ToFloat(value);
				break;
			}
			if (key == "border-top-right-radius") {
				style.Border.TopRightRadius = __ToFloat(value);
				break;
			}
			if (key == "border-bottom-right-radius") {
				style.Border.BottomRightRadius = __ToFloat(value);
				break;
			}
			if (key == "border-bottom-left-radius") {
				style.Border.BottomLeftRadius = __ToFloat(value);
				break;
			}
			if (key == "font-size") {
				style.FontSize = __ToFloat(value);
				break;
			}
			if (key == "font-family") {
				value = value.replace("\"", "");//删除双引号;
				style.FontFamily = value.unicode();
				break;
			}
			if (key == "border-width") {
				style.Border = __ToFloat(value);
				break;
			}
			if (key == "border") {
				__MakeBorder(value, style.Border, [&style](float num) {
					style.Border.Left = num;
					style.Border.Top = num;
					style.Border.Right = num;
					style.Border.Bottom = num;
					});
				break;
			}
			if (key == "border-left") {
				__MakeBorder(value, style.Border, [&style](float num) {
					style.Border.Left = num;
					});
				break;
			}
			if (key == "border-top") {
				__MakeBorder(value, style.Border, [&style](float num) {
					style.Border.Top = num;
					});
				break;
			}
			if (key == "border-right") {
				__MakeBorder(value, style.Border, [&style](float num) {
					style.Border.Right = num;
					});
				break;
			}
			if (key == "border-bottom") {
				__MakeBorder(value, style.Border, [&style](float num) {
					style.Border.Bottom = num;
					});
				break;
			}
		} while (false);
	}
	void Control::SetStyleSheet(ControlState state, const UIString& styleStr)
	{
		auto attrs = styleStr.split(";");//分割每一行样式
		if (!attrs.empty()) {
			ControlStyle& sytle = this->GetStyle(state);//获取对应状态下的Style
			for (auto& it : attrs) {
				size_t pos = it.find(":");
				if (pos == -1)continue;
				UIString key = it.substr(0, pos);
				UIString value = it.substr(pos + 1);
				key = key.trim();//去除前后空格
				value = value.trim();//去除前后空格
				this->SetStyle(sytle, key, value);//去应用每一行样式
			}
		}
	}
};
