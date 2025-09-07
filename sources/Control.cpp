#include "Control.h"
#include "ScrollBar.h"
#include "Spacer.h"
#include "IFrame.h"

namespace ezui {
	inline bool __IsValid(int value) {
		return value != 0;
	}
	inline bool __IsValid(WORD value) {
		return value != 0;
	}
	inline bool __IsValid(BYTE value) {
		return value != 0;
	}
	inline bool __IsValid(float value) {
		return std::abs(value) > EZUI_FLOAT_EPSILON;
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
	inline bool __IsValid(HCURSOR value) {
		return value != NULL;
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
		if (__IsPx(numStr, value)) {//以px为单位
			return value;
		}
		value = std::atof(numStr.c_str());
		if (numStr.rfind("%") != std::string::npos) {//判断是不是百分比
			value = value / 100.0f;//如果是百分比则返回小数点(0.0f~1.0f)
		}
		return value;//直接转数值返回
	}

	//处理url(d:/imgs/aa.png)路径
	inline Image* __MakeImage(UIString value) {
		value = value.replace("\"", "");//删除双引号;
		auto pos1 = value.find("(");
		auto pos2 = value.find(")");
		if (pos1 != std::string::npos && pos2 != std::string::npos) {
			//background-image:url(res/images/xxx.png)的方式
			value = value.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		return Image::Make(value);
	}

	//构建边框信息并回调
	inline void __MakeBorder(UIString str, Border& bd, const std::function<void(float)>& callback) {
		size_t pos1 = str.find('(');
		size_t pos2 = str.find(')');
		if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1) {
			//删除括号内的空格
			std::string inside = str.substr(pos1 + 1, pos2 - pos1 - 1);
			inside.erase(std::remove(inside.begin(), inside.end(), ' '), inside.end());
			str = str.substr(0, pos1 + 1) + inside + str.substr(pos2);
		}
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
		if(this->IsEnabled()){ \
			if ((_state & ControlState::Active) == ControlState::Active) {\
				_##_filed = this->GetStyle(ControlState::Active).##_filed;\
				if (__IsValid(_##_filed)) {\
					return _##_filed;\
				}else{\
					_##_filed = this->GetStyle(ControlState::Hover).##_filed;\
					if (__IsValid(_##_filed)) return _##_filed;\
				}\
			}\
			if ((_state & ControlState::Hover) == ControlState::Hover) {\
				_##_filed = this->GetStyle(ControlState::Hover).##_filed;\
				if (__IsValid(_##_filed)) {\
					return _##_filed;\
				}\
			}\
		}else {\
				_##_filed = this->GetStyle(ControlState::Disabled).##_filed;\
				if (__IsValid(_##_filed)) return _##_filed;\
		}\
		_##_filed = this->GetStyle(ControlState::Static).##_filed; \
		if (__IsValid(_##_filed)) {\
				return _##_filed; \
		}\
		Control* parentCtrl = this->GetParent(); \
		while (parentCtrl){ \
			_##_filed = parentCtrl->GetStyle(ControlState::Static).##_filed; \
			if (__IsValid(_##_filed)) {\
				break; \
			}\
			parentCtrl = parentCtrl->GetParent(); \
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
		if(this->IsEnabled()){ \
			if ((_state & ControlState::Active) == ControlState::Active) {\
				_##_filed = this->GetStyle(ControlState::Active).##_filed;\
				if (__IsValid(_##_filed)){\
					return _##_filed;\
				}else{\
					_##_filed = this->GetStyle(ControlState::Hover).##_filed; \
					if (__IsValid(_##_filed))return _##_filed; \
				}\
			}\
			if ((_state & ControlState::Hover) == ControlState::Hover) {\
				_##_filed = this->GetStyle(ControlState::Hover).##_filed;\
				if (__IsValid(_##_filed))return _##_filed;\
			}\
		}else {\
				_##_filed = this->GetStyle(ControlState::Disabled).##_filed;\
				if (__IsValid(_##_filed)) return _##_filed;\
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
		if(this->IsEnabled()){ \
			if ((_state & ControlState::Active) == ControlState::Active) {\
				_##_filed = this->GetStyle(ControlState::Active).##_filed1.##_filed;\
				if (__IsValid(_##_filed)){\
					return _##_filed;\
				}else{\
					_##_filed = this->GetStyle(ControlState::Hover).##_filed1.##_filed;\
					if (__IsValid(_##_filed)) return _##_filed;\
				}\
			}\
			if ((_state & ControlState::Hover) == ControlState::Hover) {\
				_##_filed = this->GetStyle(ControlState::Hover).##_filed1.##_filed;\
				if (__IsValid(_##_filed)) return _##_filed;\
			}\
		}else {\
				_##_filed = this->GetStyle(ControlState::Disabled).##_filed1.##_filed;\
				if (__IsValid(_##_filed)) return _##_filed;\
		}\
		return this->GetStyle(ControlState::Static).##_filed1.##_filed; \
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
	UI_BORDER_BINDFUNC(StrokeStyle, Border, Style);

	UI_STYLE_BINDFUNC(Color, BackColor);
	UI_STYLE_BINDFUNC(Image*, ForeImage);
	UI_STYLE_BINDFUNC(Image*, BackImage);
	UI_STYLE_BINDFUNC(float, Angle);
	UI_STYLE_BINDFUNC(HCURSOR, Cursor);

	UI_SUPER_STYLE_BINDFUNC(int, FontSize);
	UI_SUPER_STYLE_BINDFUNC(Color, ForeColor);
	UI_SUPER_STYLE_BINDFUNC(std::wstring, FontFamily);

	WindowContext* Control::GetWindowContext()
	{
		return (WindowContext*)UI_GET_USERDATA(Hwnd());
	}

	IFrame* Control::GetFrame()
	{
		IFrame* frame = NULL;
		Control* parent = this;
		//依次往上父控件看看有没有当前控件是否在内联页面中
		while (parent)
		{
			if (parent->IsFrame()) {
				frame = (IFrame*)parent;
				break;
			}
			parent = parent->m_parent;
		}
		return frame;
	}

	Control::Control(Object* parentObject) :Object(parentObject)
	{
	}
	void Control::OnChildPaint(PaintEventArgs& args)
	{
		m_viewControls.clear();
		//绘制子控件
		Rect rect(0, 0, Width(), Height());
		for (auto& it : m_controls) {
			if (rect.IntersectsWith(it->GetRect())) {
				m_viewControls.push_back(it);
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
		Color backgroundColor = this->GetBackColor();
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
		if (border.Color.GetValue() == 0) return;//边框无效颜色不绘制
		if (border.Style == StrokeStyle::None)return;//未指定边框风格 不允许绘制
		int borderLeft = border.Left;
		int borderTop = border.Top;
		int borderRight = border.Right;
		int borderBottom = border.Bottom;
		int topLeftRadius = border.TopLeftRadius;
		int topRightRadius = border.TopRightRadius;
		int bottomRightRadius = border.BottomRightRadius;
		int bottomLeftRadius = border.BottomLeftRadius;

		//指定边框风格
		e.Graphics.SetStrokeStyle(border.Style);
		//规则的矩形
		if (topLeftRadius == 0 && topRightRadius == 0 && bottomLeftRadius == 0 && bottomRightRadius == 0) {
			bool hasBorder = borderLeft || borderTop || borderRight || borderBottom;
			if (!hasBorder) return;//边框为0不绘制
			e.Graphics.SetColor(border.Color);
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
			int value1 = borderLeft > borderTop ? borderLeft : borderTop;
			int value2 = borderRight > borderBottom ? borderRight : borderBottom;
			int maxBorder = value1 > value2 ? value1 : value2;
			if (maxBorder <= 0) return;
			float half = maxBorder / 2.0f;
			e.Graphics.SetColor(border.Color);

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
		if ((_state & ControlState::Disabled) == ControlState::Disabled) {
			return this->DisabledStyle;
		}
		else if ((_state & ControlState::Hover) == ControlState::Hover) {
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
			if (this->ApplyStyleProperty(attrName, attrValue)) {
				break;
			}
			if (attrName == "style") {
				//加载内联样式
				this->SetStyleSheet(ControlState::Static, attrValue);
				break;
			}
			if (attrName == "name" || attrName == "id") {
				this->Name = attrValue;
				break;
			}
			if (attrName == "location") {
				auto arr = attrValue.split(",");
				int x = __ToFloat(arr[0]);
				int y = __ToFloat(arr[1]);
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
					int width = __ToFloat(arr[0]);
					int height = __ToFloat(arr[1]);
					this->SetFixedSize(Size(width, height));
				}
				break;
			}
			if (attrName == "rect" && !attrValue.empty()) {
				auto rectStr = attrValue.split(",");
				Rect rect;
				rect.X = __ToFloat(rectStr[0]);
				rect.Y = __ToFloat(rectStr[1]);
				rect.Width = __ToFloat(rectStr[2]);
				rect.Height = __ToFloat(rectStr[3]);
				this->SetRect(rect);
				break;
			}
			if (attrName == "visible") {
				this->m_bVisible = (attrValue != "false");
				break;
			}
			if (attrName == "tips" || attrName == "title") {
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
					this->SetEnabled(true);
					break;
				}
				if (attrValue == "false") {
					this->SetEnabled(false);
					break;
				}
			}
			if (attrName == "disabled") {
				this->SetEnabled(false);
				break;
			}
			if (attrName == "scrollbar") {
				ScrollBar* sb = this->GetScrollBar();
				if (sb) {
					sb->SetAttribute("name", attrValue);
				}
				break;
			}
			if (attrName == "event") {
				if (attrValue == "none") {
					this->SetHitTestVisible(false);
				}
				break;
			}
		} while (false);
	}
	const ControlCollection& Control::GetCachedViewControls()
	{
		return this->m_viewControls;
	}

	Control* Control::GetParent() {
		return m_parent;
	}

	float Control::GetScale()
	{
		return this->m_scale;
	}

	HWND Control::Hwnd()
	{
		return m_hWnd;
	}
	void Control::SetHwnd(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	int Control::X()
	{
		return m_rect.X;
	}
	int Control::Y()
	{
		return m_rect.Y;
	}
	int Control::Width()
	{
		return m_rect.Width;
	}
	int Control::Height()
	{
		return m_rect.Height;
	}
	void Control::SetX(int X) {
		SetLocation({ X,m_rect.Y });
	}
	void Control::SetY(int Y) {
		SetLocation({ m_rect.X,Y });
	}
	void Control::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, m_rect.Width, m_rect.Height));
	}
	void Control::SetWidth(int width) {
		SetSize({ width,m_rect.Height });
	}
	void Control::SetHeight(int height) {
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
	void Control::SetFixedWidth(int fixedWidth)
	{
		m_fixedSize.Width = fixedWidth;
		SetRect({ m_rect.X,m_rect.Y,fixedWidth,m_rect.Height });
	}
	void Control::SetFixedHeight(int fixedHeight)
	{
		m_fixedSize.Height = fixedHeight;
		SetRect({ m_rect.X,m_rect.Y,m_rect.Width,fixedHeight });
	}
	void Control::SetRateWidth(float rateWidth)
	{
		m_rateSize.Width = rateWidth;
		if (m_parent) {
			SetFixedWidth(m_parent->Width() * rateWidth);
		}
	}
	void Control::SetRateHeight(float rateHeight)
	{
		m_rateSize.Height = rateHeight;
		if (m_parent) {
			SetFixedHeight(m_parent->Height() * rateHeight);
		}
	}
	void Control::SetFixedSize(const Size& size)
	{
		m_fixedSize.Width = size.Width;
		m_fixedSize.Height = size.Height;
		SetRect({ m_rect.X,m_rect.Y,size.Width,size.Height });
	}
	int Control::GetFixedWidth()
	{
		if (m_parent) {
			if (m_rateSize.Width > EZUI_FLOAT_EPSILON) {
				m_fixedSize.Width = m_parent->Width() * m_rateSize.Width + 0.5;
			}
		}
		return m_fixedSize.Width;
	}
	int Control::GetFixedHeight()
	{
		if (m_parent) {
			if (m_rateSize.Height > EZUI_FLOAT_EPSILON) {
				m_fixedSize.Height = m_parent->Height() * m_rateSize.Height + 0.5;
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
		int x = rect.X;
		int y = rect.Y;
		while ((pCtrl = pCtrl->m_parent))
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
		if (dockStyle != this->m_dock && m_parent) {
			m_parent->TryPendLayout();
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
			if (m_parent) {
				m_parent->RefreshLayout();
				m_parent->Invalidate();
			}
			return;
		}
		if (IsAutoWidth() && Width() != m_contentSize.Width) {
			this->SetFixedWidth(m_contentSize.Width);
			this->EndLayout();
			this->RefreshLayout();
			if (m_parent) {
				m_parent->RefreshLayout();
				m_parent->Invalidate();
			}
			return;
		}
		this->EndLayout();
	}
	void Control::OnLayout() {
		m_contentSize.Width = 0;
		m_contentSize.Height = 0;
		int _width;
		int _height;
		for (auto& it : GetControls()) {

			if (m_parent) {
				//设置了宽高百分比的控件
				if (it->m_rateSize.Width > EZUI_FLOAT_EPSILON) {
					it->SetFixedWidth(m_parent->Width() * it->m_rateSize.Width);
				}
				if (it->m_rateSize.Height > EZUI_FLOAT_EPSILON) {
					it->SetFixedHeight(m_parent->Height() * it->m_rateSize.Height);
				}
			}

			//设置了dockstyle的控件
			if (it->GetDockStyle() == DockStyle::Fill) {
				it->SetRect(Rect{ 0,0,Width(),Height() });
			}
			else if (it->GetDockStyle() == DockStyle::Vertical) {
				it->SetRect(Rect{ it->X(),0,it->Width(),Height() });
			}
			else if (it->GetDockStyle() == DockStyle::Horizontal) {
				it->SetRect(Rect{ 0,it->Y(),Width(),it->Height() });
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
	void Control::SendEvent(const EventArgs& arg)
	{
		this->OnEvent((EventArgs&)arg);
	}
	void Control::OnEvent(EventArgs& arg)
	{
		if (!this->IsEnabled() && ((arg.EventType & Event::OnMouseEvent) == arg.EventType || (arg.EventType & Event::OnKeyBoardEvent) == arg.EventType))
		{
			// 禁用状态下拦截鼠标或键盘事件
			return;
		}
		bool isRemove = false;
		this->m_bRemove = &isRemove;
		do
		{
			if (arg.EventType == Event::OnPaint) {
				if (IsVisible()) {
					this->OnPaintBefore((PaintEventArgs&)arg);
				}
				return; //绘制消息特殊直接return
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
			if (isRemove) break;//控件已被删除 不在继续下发通知
			//发送给控件自身事件处理器(如果用户绑定了此回调函数,则不再进入主窗口的OnNotify函数)
			if (this->EventHandler) {
				this->EventHandler(this, arg);
				if (isRemove) break;
			}
			if ((this->NotifyFlags & arg.EventType) == arg.EventType) {
				//向Frame层通知事件
				IFrame* frame = this->GetFrame();
				if (frame) {
					frame->OnNotify(this, arg);
				}
			}
		} while (false);

		if (!isRemove) {
			this->m_bRemove = NULL;
		};
	}

	bool Control::ApplyStyleProperty(const UIString& key, const UIString& value) {
		do
		{
			if (key == "x") {
				this->SetLocation({ (int)(__ToFloat(value) * this->GetScale() + 0.5) ,this->Y() });
				break;
			}
			if (key == "y") {
				this->SetLocation({ this->X(),(int)(__ToFloat(value) * this->GetScale() + 0.5) });
				break;
			}
			if (key == "width") {
				if (value == "auto") {
					this->SetAutoWidth(true);//自动宽度
				}
				else {
					if (value.rfind("%") != std::string::npos) {
						this->SetRateWidth(__ToFloat(value));//基于父控件的百分比
					}
					else {
						this->SetFixedWidth(__ToFloat(value) * this->GetScale() + 0.5);//如果单独设置了宽高那就是绝对宽高了
					}
				}
				break;
			}
			if (key == "height") {
				if (value == "auto") {
					this->SetAutoHeight(true);//自动高度
				}
				else {
					if (value.rfind("%") != std::string::npos) {
						this->SetRateHeight(__ToFloat(value));//基于父控件的百分比
					}
					else {
						this->SetFixedHeight(__ToFloat(value) * this->GetScale() + 0.5);//如果单独设置了宽高那就是绝对宽高了
					}
				}
				break;
			}
			if (key == "margin-left") {
				this->Margin.Left = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "margin-top") {
				this->Margin.Top = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "margin-right") {
				this->Margin.Right = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "margin-bottom") {
				this->Margin.Bottom = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "margin") {//遵循web前端的规则
				auto strs = value.replace(' ', ',').split(",");//去掉空格并转为数组
				if (strs.size() == 1) {
					this->Margin = __ToFloat(strs[0]) * this->GetScale() + 0.5;
					break;
				}
				if (strs.size() == 2) {
					this->Margin.Top = this->Margin.Bottom = __ToFloat(strs[0]) * this->GetScale() + 0.5;
					this->Margin.Left = this->Margin.Right = __ToFloat(strs[1]) * this->GetScale() + 0.5;
					break;
				}
				if (strs.size() == 4) {
					this->Margin.Top = __ToFloat(strs[0]) * this->GetScale() + 0.5;
					this->Margin.Right = __ToFloat(strs[1]) * this->GetScale() + 0.5;
					this->Margin.Bottom = __ToFloat(strs[2]) * this->GetScale() + 0.5;
					this->Margin.Left = __ToFloat(strs[3]) * this->GetScale() + 0.5;
					break;
				}
				break;
			}
			if (key == "display") {
				this->m_bVisible = (value != "none");
				break;
			}
			return false;//未处理
		} while (false);
		return true;//处理成功
	}
	//专门处理键盘消息的
	void Control::OnKeyBoardEvent(const KeyboardEventArgs& _args) {
		do
		{
			KeyboardEventArgs& args = (KeyboardEventArgs&)_args;
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
				this->State = ControlState::Hover;
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
				this->m_pressed = true;
				this->State = ControlState::Active;
				OnMouseDown(args);
				break;
			}
			case Event::OnMouseUp: {
				this->m_pressed = false;
				OnMouseUp(args);
				break;
			}
			case Event::OnMouseLeave: {
				this->m_pressed = false;
				this->State = ControlState::Static;
				OnMouseLeave(args);
				break;
			}
			default:
				break;
			}
		} while (false);
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
		this->OnPaint(args);
		//通知到主窗口OnNotify或者IFrame的OnNotify函数中
		WindowContext* publicData = NULL;
		if ((this->NotifyFlags & Event::OnPaint) == Event::OnPaint) {
			//向Frame层通知事件
			IFrame* frame = this->GetFrame();
			if (frame) {
				frame->OnNotify(this, args);
			}
		}
		//通知到自身控件的处理器
		if (this->EventHandler) {
			this->EventHandler(this, args);
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
		args.PopLayer();//弹出纹理层

#ifdef _DEBUG
		if ((publicData = this->GetWindowContext()) && publicData->Debug) {
			float width = 1 * this->GetScale();
			Color color = publicData->DebugColor;
			color.SetA(127);
			pt.SetColor(color);

			//绘制虚线
			pt.SetStrokeStyle(StrokeStyle::Dash);

			//绘制出控件边框
			pt.DrawRectangle(RectF(0, 0, clientRect.Width, clientRect.Height), 0, width);

			//绘制出边距 类似安卓那样显示
			// 左边距
			if (Margin.Left) {
				pt.FillRectangle(RectF(0 - Margin.Left, 0, Margin.Left, clientRect.Height));
			}
			// 上边距
			if (Margin.Top) {
				pt.FillRectangle(RectF(0, 0 - Margin.Top, clientRect.Width, Margin.Top));
			}
			// 右边距
			if (Margin.Right) {
				pt.FillRectangle(RectF(clientRect.Width, 0, Margin.Right, clientRect.Height));
			}
			// 下边距
			if (Margin.Bottom) {
				pt.FillRectangle(RectF(0, clientRect.Height, clientRect.Width, Margin.Bottom));
			}
		}
#endif
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
		if (!IsFloatEqual(scale, 1.0f)) {
			needScale = true;
			int fw = this->GetFixedWidth() * scale + 0.5;
			int fh = this->GetFixedHeight() * scale + 0.5;

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
			this->DisabledStyle.Scale(scale);
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
		for (auto& it : m_controls) {
			it->m_parent = NULL;//告诉孩子们爸爸已经没了
		}
		if (m_parent && !m_parent->IsDestroying()) {
			m_parent->Remove(this);
		}
		auto* publicData = GetWindowContext();
		//清除绑定信息
		if (publicData) {
			publicData->CleanControl(this);
		}
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

	int Control::IndexOf(Control* childCtl)
	{
		const auto& pControls = this->GetControls();
		int pos = 0;
		for (auto itor = pControls.begin(); itor != pControls.end(); ++itor)
		{
			if ((*itor)->IsSpacer()) {
				continue;
			}
			if ((*itor) == childCtl) {
				return pos;
			}
			++pos;
		}
		return -1;
	}
	Control* Control::Add(Control* ctl) {
#ifdef _DEBUG
		auto itor = std::find(m_controls.begin(), m_controls.end(), ctl);
		if (itor != m_controls.end()) {
			ASSERT(!"The control already exists and cannot be added repeatedly");
		}
#endif
		this->ApplyParentStyles();//样式匹配
		if (ctl->IsSpacer()) {//是否为弹簧控件
			this->Attach(ctl);
		}
		m_controls.push_back(ctl);
		ctl->SetHwnd(this->Hwnd());
		ctl->m_parent = this;

		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendEvent(DpiChangeEventArgs(this->GetScale()));
		}

		ctl->TryPendLayout();
		this->TryPendLayout();//添加控件需要将布局重新挂起
		return ctl;
	}
	Control* Control::Insert(int pos, Control* ctl)
	{
#ifdef _DEBUG
		{
			auto itor = std::find(m_controls.begin(), m_controls.end(), ctl);
			if (itor != m_controls.end()) {
				ASSERT(!"The control already exists and cannot be added repeatedly");
			}
		}
#endif
		this->ApplyParentStyles();//样式匹配
		if (ctl->IsSpacer()) {//是否为弹簧控件
			this->Attach(ctl);
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
		ctl->m_parent = this;
		if (ctl->GetScale() != this->GetScale()) {
			ctl->SendEvent(DpiChangeEventArgs(this->GetScale()));
		}
		ctl->TryPendLayout();
		this->TryPendLayout();//添加控件需要将布局重新挂起
		return ctl;
	}
	void Control::SetParent(Control* parentCtl)
	{
		parentCtl->Add(this);
	}

	Control* Control::Append(const UIString& xmlStr) {
		UILoader* loader = new UILoader(this);
		loader->LoadXml(xmlStr.c_str(), xmlStr.size());
		return this->Add(loader->GetRoot());
	}

	Control* Control::Prepend(const UIString& xmlStr)
	{
		UILoader* loader = new UILoader(this);
		loader->LoadXml(xmlStr.c_str(), xmlStr.size());
		return this->Insert(0, loader->GetRoot());
	}

	void Control::Remove(Control* ctl, bool freeCtrl)
	{
		//寻找控件看是否包含
		auto itor = ::std::find(m_controls.begin(), m_controls.end(), ctl);
		if (itor != m_controls.end()) {
			ctl->OnRemove();
			this->TryPendLayout();//移除控件需要将布局重新挂起
			m_controls.erase(itor);
			auto itor2 = ::std::find(m_viewControls.begin(), m_viewControls.end(), ctl);
			if (itor2 != m_viewControls.end()) {
				m_viewControls.erase(itor2);
			}
			if (freeCtrl) {
				delete ctl;
			}
		}
		//子对象存在相同的对象也要跟随移除
		this->Detach(ctl);
	}
	void Control::OnRemove()
	{
		if (m_bRemove) {
			*m_bRemove = true;
		}
		for (auto& it : m_controls) {
			it->OnRemove();
		}
		auto* publicData = GetWindowContext();
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
			if (it->IsFrame()) {
				continue;//对IFrame内的控件进行隔离
			}
			auto ctl = it->FindControl(ctlName);
			if (ctl) return ctl;
		}
		return NULL;
	}
	ControlCollection Control::FindControl(const UIString& attrName, const UIString& attrValue)
	{
		ControlCollection ctls;
		if (attrName.empty() || attrValue.empty()) {
			return ctls;
		}
		for (auto& it : (this->m_controls))
		{
			if (it->GetAttribute(attrName) == attrValue) {
				ctls.push_back(it);
			}
			if (it->IsFrame()) {
				continue;//对IFrame内的控件进行隔离
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
	ControlCollection Control::FindChild(const UIString& attrName, const UIString& attrValue)
	{
		ControlCollection ctls;
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
		int swapCount = 0;
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

	void Control::SetEnabled(bool flag) {
		this->m_enabled = flag;
	}
	void Control::SetDisabled(bool flag) {
		this->m_enabled = !flag;
	}
	bool Control::IsEnabled() {
		return this->m_enabled;
	}

	void Control::SetVisible(bool flag) {
		if (flag != this->m_bVisible && this->m_parent) {
			this->m_parent->TryPendLayout();
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

	void Control::SetHitTestVisible(bool bEnable) {
		m_hitTestEnabled = bEnable;
	}

	bool Control::IsHitTestVisible() {
		return m_hitTestEnabled;
	}

	bool Control::Invalidate() {
		auto* publicData = GetWindowContext();
		if (publicData) {
			if (m_parent && this->IsPendLayout()) {
				return m_parent->Invalidate();
			}
			float angle = this->GetAngle();
			if (!(angle == 0 || angle == 180) && m_parent) {
				return m_parent->Invalidate();
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
			auto* publicData = GetWindowContext();
			if (publicData) {
				publicData->Refresh();//立即更新全部无效区域
			}
		}
	}
	void Control::ApplyChildStyles(const std::list<ezui::Style>& styles) {
		ezui::ApplyStyle(this, styles);      // 先对当前子控件应用
		for (auto& child : this->m_controls) {
			child->ApplyChildStyles(styles); // 再递归应用到子控件的子控件
		}
	}
	void Control::ApplyParentStyles()
	{
		//依次往父控件找(找到合适的样式就应用上)
		Control* parent = this->m_parent;
		while (parent) {
			if (!parent->m_styles.empty()) {
				ezui::ApplyStyle(this, parent->m_styles);
			}
			if (parent->IsFrame()) {
				break;// 找到第一个 Frame 后就停止,不再继续向上
			}
			parent = parent->m_parent;
		}
	}
	void Control::SetStyleSheet(const UIString& styleStr)
	{
		std::list<ezui::Style> styles;
		//分析出样式
		ezui::AnalysisStyle(styleStr, &styles);
		//先应用样式
		if (!styles.empty()) {
			ApplyChildStyles(styles);  // 递归应用到所有子控件
		}
		//保存样式到控件
		m_styles.splice(m_styles.end(), styles);  // 把 styles 的所有元素移动到 this->Styles 末尾
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
		if (flag != this->m_bAutoWidth && m_parent) {
			m_parent->TryPendLayout();
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
		if (flag != this->m_bAutoHeight && m_parent) {
			m_parent->TryPendLayout();
		}
		if (flag && Height() == 0) {
			m_rect.Height = 1;
		}
		this->m_bAutoHeight = flag;
	}
	void Control::SetContentWidth(int width)
	{
		this->m_contentSize.Width = width;
	}
	void Control::SetContentHeight(int height)
	{
		this->m_contentSize.Height = height;
	}
	void Control::SetContentSize(const Size& size)
	{
		this->m_contentSize = size;
	}
	void Control::ComputeClipRect()
	{
		if (m_parent) {
			Rect& ClipRectRef = *(Rect*)(&this->m_viewRect);//引用父控件的裁剪区域
			Rect::Intersect(ClipRectRef, this->GetClientRect(), m_parent->m_viewRect);//自身和父控件对比较裁剪区域
		}
		else {
			Rect& ClipRectRef = *(Rect*)(&this->m_viewRect);
			ClipRectRef = this->GetClientRect();
		}
	}
	const ControlCollection& Control::GetControls()
	{
		return m_controls;
	}
	Control* Control::GetControl(int pos)
	{
		int _pos = 0;
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
			pControl = pControl->m_parent;
		}
		return false;
	}

	void Control::Clear()
	{
		this->Clear(false);
	}

	void Control::Clear(bool freeChilds)
	{
		auto itor = m_controls.begin();
		while (itor != m_controls.end())//循环清除控件
		{
			Control* it = *itor;
			itor = m_controls.erase(itor); //erase返回下一个有效迭代器
			it->OnRemove();
			if (freeChilds) {
				this->Detach(it);//子对象存在相同的对象也要跟随移除
				delete it;
			}
		}

		this->m_viewControls.clear();//清空可见控件
		this->TryPendLayout();//挂起布局
		ScrollBar* scrollBar = this->GetScrollBar();
		if (scrollBar) {
			scrollBar->Reset();
		}
	}
	bool Control::IsSpacer()
	{
		return false;
	}
	bool Control::IsFrame()
	{
		return false;
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
		this->Invalidate();
	}
	void Control::OnMouseDown(const MouseEventArgs& args)
	{
		this->Invalidate();
	}
	void Control::OnMouseUp(const MouseEventArgs& args)
	{
		this->Invalidate();
	}
	void Control::OnMouseLeave(const MouseEventArgs& args)
	{
		this->Invalidate();
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
		if (m_parent) {
			m_parent->TryPendLayout();//将父控件挂起
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
			if (this->ApplyStyleProperty(key, value)) {
				break;
			}
			if (key == "pointer-events") {
				if (value == "none") {
					//忽略鼠标键盘事件 将直接穿透
					this->SetHitTestVisible(false);
				}
				break;
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
					style.BackImage->DrawPosition.X = x * this->GetScale() + 0.5;

					float y = 0;
					__IsPx(pxs[1], y);
					style.BackImage->DrawPosition.Y = y * this->GetScale() + 0.5;
				}
				break;
			}
			if (key == "background-size" || key == "background-image-size") {
				if (value == "auto" && style.BackImage) {
					style.BackImage->SizeMode = ImageSizeMode::Original;
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
					style.ForeImage->SizeMode = ImageSizeMode::Original;
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
				style.Border.Radius = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "border-top-left-radius") {
				style.Border.TopLeftRadius = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "border-top-right-radius") {
				style.Border.TopRightRadius = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "border-bottom-right-radius") {
				style.Border.BottomRightRadius = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "border-bottom-left-radius") {
				style.Border.BottomLeftRadius = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "font-size") {
				style.FontSize = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "font-family") {
				value = value.replace("\"", "");//删除双引号;
				style.FontFamily = value.unicode();
				break;
			}
			if (key == "border-width") {
				style.Border = __ToFloat(value) * this->GetScale() + 0.5;
				break;
			}
			if (key == "border") {
				__MakeBorder(value, style.Border, [&style, this](float num) {
					style.Border.Left = num * this->GetScale() + 0.5;
					style.Border.Top = num * this->GetScale() + 0.5;
					style.Border.Right = num * this->GetScale() + 0.5;
					style.Border.Bottom = num * this->GetScale() + 0.5;
					});
				break;
			}
			if (key == "border-left") {
				__MakeBorder(value, style.Border, [&style, this](float num) {
					style.Border.Left = num * this->GetScale() + 0.5;
					});
				break;
			}
			if (key == "border-top") {
				__MakeBorder(value, style.Border, [&style, this](float num) {
					style.Border.Top = num * this->GetScale() + 0.5;
					});
				break;
			}
			if (key == "border-right") {
				__MakeBorder(value, style.Border, [&style, this](float num) {
					style.Border.Right = num * this->GetScale() + 0.5;
					});
				break;
			}
			if (key == "border-bottom") {
				__MakeBorder(value, style.Border, [&style, this](float num) {
					style.Border.Bottom = num * this->GetScale() + 0.5;
					});
				break;
			}
		} while (false);
	}
	void Control::SetStyleSheet(ControlState state, const UIString& styleListStr)
	{
		UIString styleStr = styleListStr.trim();
		if (styleStr.find("{") == 0 && styleStr.rfind("}") == (styleStr.size() - 1)) {
			styleStr = styleStr.substr(1, styleStr.size() - 2);//去除括号
		}
		auto attrs = styleStr.split(";");//分割每一行样式
		if (!attrs.empty()) {
			ControlStyle& sytle = this->GetStyle(state);//获取对应状态下的Style
			for (auto& it : attrs) {
				size_t pos = it.find(":");
				if (pos == std::string::npos) {
					continue;
				}
				UIString key = it.substr(0, pos);
				UIString value = it.substr(pos + 1);
				key = key.trim();//去除前后空格
				value = value.trim();//去除前后空格
				this->SetStyle(sytle, key, value);//去应用每一行样式
			}
		}
	}
	bool Control::IsPressed()
	{
		return m_pressed;
	}
};
