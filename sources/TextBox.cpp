#include "TextBox.h"
#undef min
#undef max
namespace ezui {
	TextBox::TextBox(Object* parentObject) :Control(parentObject)
	{
		Init();
	}
	TextBox::~TextBox() {
		m_timer->Stop();
		if (m_textLayout) { delete m_textLayout; }
		if (m_font) { delete m_font; }
	}
	void TextBox::Init()
	{
		this->GetScrollBar()->SetWidth(5);
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [=](int offset) {
			this->Offset(offset);
			};

		Style.Cursor = LoadCursor(Cursor::IBEAM);

		m_timer = new Timer(this);
		m_timer->Interval = 500;
		m_timer->Tick = [this](Timer* t) {

			HWND hWnd = this->Hwnd();//在对象析构前获取句柄
			BeginInvoke([this, hWnd]() {//捕获hWnd
				if (!::IsWindow(hWnd))return;//如果窗口已经销毁 则不往下执行

				if (!this->IsEnabled() || this->IsReadOnly()) {
					m_bCareShow = false;
					return;
				}
				if (!m_careRect.IsEmptyArea() && m_focus) {
					m_bCareShow = !m_bCareShow;
					this->Invalidate();
				}
				});
			};
	}
	void TextBox::OnRemove() {
		__super::OnRemove();
		m_timer->Stop();
	}
	void TextBox::SetAutoWidth(bool flag)
	{
		//需要屏蔽
	}
	void TextBox::SetAutoHeight(bool flag)
	{
		//需要屏蔽
	}
	void TextBox::OnKeyChar(const KeyboardEventArgs& arg)
	{
		__super::OnKeyChar(arg);
		WPARAM wParam = arg.wParam;
		LPARAM lParam = arg.lParam;

		if (wParam < 32)return;//控制字符
		if (IsReadOnly()) return;//只读
		WCHAR buf[2]{ (WCHAR)wParam ,0 };//
		InsertUnicode(std::wstring(buf));//插入新的字符
		Analysis();//分析字符串
		Invalidate();//刷新
	}

#undef max
#undef min
	void TextBox::BuildSelectedRect() {
		m_selectRects.clear();
		if (m_textLayout) {
			//获取起点和终点
			Point point1, point2;
			if ((m_A_TextPos + m_A_isTrailingHit) < (m_B_TextPos + m_B_isTrailingHit)) {
				point1 = m_textLayout->HitTestTextPosition(m_A_TextPos, m_A_isTrailingHit);
				point2 = m_textLayout->HitTestTextPosition(m_B_TextPos, m_B_isTrailingHit);
			}
			else {
				point2 = m_textLayout->HitTestTextPosition(m_A_TextPos, m_A_isTrailingHit);
				point1 = m_textLayout->HitTestTextPosition(m_B_TextPos, m_B_isTrailingHit);
			}
			// 中心偏移用于 hit test 保证在行内
			float offsetY = m_textLayout->GetFontHeight() / 2.0f;
			point1.Y += offsetY;
			point2.Y += offsetY;

			// 获取每行矩形
			auto lineRects = m_textLayout->GetLineRects();

			for (auto& lr : lineRects) {
				// 首行
				if (point1.Y >= lr.Y && point1.Y <= lr.Y + lr.Height) {
					if (point2.Y <= lr.Y + lr.Height) {
						// 同一行
						float width = std::max(0.0f, float(point2.X - point1.X));
						m_selectRects.push_back(Rect(point1.X, lr.Y, width, lr.Height));
					}
					else {
						// 跨行首行 从 point1 到行末
						float width = std::max(0.0f, lr.X + lr.Width - point1.X);
						m_selectRects.push_back(Rect(point1.X, lr.Y, width, lr.Height));
					}
				}
				// 末行
				else if (point2.Y >= lr.Y && point2.Y <= lr.Y + lr.Height) {
					float width = std::max(0.0f, point2.X - lr.X);
					m_selectRects.push_back(Rect(lr.X, lr.Y, width, lr.Height));
				}
				// 中间整行
				else if (point1.Y < lr.Y && point2.Y > lr.Y + lr.Height) {
					m_selectRects.push_back(lr);
				}
			}
		}
	}

	bool TextBox::SelectedAll() {
		if (m_textLayout && !m_text.empty()) {
			m_pointA = Point{ 0,0 };
			m_A_isTrailingHit = FALSE;
			m_A_TextPos = 0;

			m_pointB = Point{ m_fontBox.Width ,0 };
			m_B_isTrailingHit = TRUE;
			m_B_TextPos = m_text.size() - 1;

			BuildSelectedRect();
			return true;
		}
		return false;
	}
	bool TextBox::GetSelectedRange(int* outPos, int* outCount) {
		if (m_selectRects.size() > 0) {
			int pos, count;
			if ((m_A_TextPos + m_A_isTrailingHit) < (m_B_TextPos + m_B_isTrailingHit)) {
				int pos1 = m_A_TextPos;
				if (m_A_isTrailingHit == 1) {
					pos1 += 1;
				}
				int pos2 = m_B_TextPos;
				if (m_B_isTrailingHit == 0) {
					pos2 -= 1;
				}
				pos = pos1;
				count = std::abs(pos2 - pos1) + 1;
			}
			else {
				int pos1 = m_A_TextPos;
				if (m_A_isTrailingHit == 0) {
					pos1 -= 1;
				}
				int pos2 = m_B_TextPos;
				if (m_B_isTrailingHit == 1) {
					pos2 += 1;
				}
				pos = pos2;
				count = std::abs(pos2 - pos1) + 1;
			}
			*outPos = pos;
			*outCount = count;
			if (*outCount > 0) {
				return true;
			}
		}
		return false;
	}
	void TextBox::InsertUnicode(const std::wstring& str) {
		DeleteRange();//先删除是否有选中的区域
		if (m_textPos < 0)m_textPos = 0;
		if (m_textPos > (int)m_text.size()) {
			m_textPos = m_text.size();
		}
		m_text.insert(m_textPos, str);
		m_textPos += str.size();
		if (TextChanged) {
			TextChanged(UIString(m_text));
		}
	}
	bool TextBox::DeleteRange() {
		int pos, count;
		if (GetSelectedRange(&pos, &count)) {//删除选中的
			//isTrailingHit = FALSE;
			m_textPos = pos;
			m_text.erase(pos, count);
			if (TextChanged) {
				TextChanged(UIString(m_text));
			}
			return true;
		}
		return false;
	}
	bool TextBox::Copy() {
		int pos, count;
		if (!GetSelectedRange(&pos, &count))return false;
		std::wstring wBuf(m_text.substr(pos, count));
		return ezui::CopyToClipboard(wBuf, Hwnd());
	}
	bool TextBox::Paste() {
		std::wstring wBuf;
		bool bRet = ezui::GetClipboardData(&wBuf, Hwnd());
		UIString u8Str(wBuf);
		if (!m_multiLine) {
			//行编辑框不允许有换行符
			ui_text::Replace(&u8Str, "\r", "");
			ui_text::Replace(&u8Str, "\n", "");
		}
		InsertUnicode(u8Str.unicode());//插入新的字符
		return bRet;
	}
	void TextBox::OnBackspace() {
		if (m_text.size() <= 0)return;

		if (!DeleteRange()) {//先看看有没有有选中的需要删除
			//否则删除单个字符
			m_textPos--;
			if (m_textPos > -1) {
				m_text.erase(m_textPos, 1);
				if (TextChanged) {
					TextChanged(UIString(m_text));
				}
			}
		}
	}
	void TextBox::OnKeyDown(const KeyboardEventArgs& arg)
	{
		__super::OnKeyDown(arg);
		WPARAM wParam = arg.wParam;
		LPARAM lParam = arg.lParam;

		//判断是否按下shift+enter
		if (IsMultiLine() && (GetKeyState(VK_SHIFT) & 0x8000) && wParam == VK_RETURN) {
			InsertUnicode(L"\n");//插入换行符
			Analysis();//分析字符串
			Invalidate();//刷新
			return;
		}
		//判断是否按下Ctrl键
		if (GetKeyState(VK_CONTROL) & 0x8000) {
			do
			{
				if (wParam == 'A') {//ctrl+a全选
					SelectedAll();//全选
					Invalidate();//刷新
					break;
				}
				if (wParam == 'C') {//ctrl+c 复制
					Copy();
					break;
				}
				if (wParam == 'X') {//ctrl+x 剪切
					if (IsReadOnly()) {
						break;
					}
					if (Copy()) {
						DeleteRange();//因为是剪切 所以要删除选中的这段
						Analysis();
						Invalidate();//刷新
					}
					break;
				}
				if (wParam == 'V') {//ctrl+v 粘贴
					if (IsReadOnly()) {
						break;
					}
					Paste();
					Analysis();//分析字符串
					Invalidate();//刷新
					break;
				}
				if (wParam == 'Z') {//ctrl+z撤销
					if (IsReadOnly()) {
						break;
					}
					break;
				}
			} while (false);
			return;
		}
		if (wParam == VK_BACK) { //退格键(删除字符)
			if (IsReadOnly()) {
				return;
			}
			OnBackspace();//退格键的操作在里面
			Analysis();//重新分析
			Invalidate();//刷新
			return;
		}
		if (wParam == VK_LEFT) {
			m_textPos--;
			m_bCareShow = true;
			m_selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
		if (wParam == VK_RIGHT) {
			++m_textPos;
			m_bCareShow = true;
			m_selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
	}

	void TextBox::Analysis()
	{
		if (m_text.size() > (size_t)(this->m_maxLen)) {
			m_text.erase((size_t)(this->m_maxLen));
		}
		m_scrollX = 0;
		m_scrollY = 0;
		m_pointA = Point();
		m_A_isTrailingHit = 0;
		m_A_TextPos = 0;
		m_pointB = Point();
		m_B_isTrailingHit = 0;
		m_B_TextPos = 0;
		m_careRect = Rect();
		m_selectRects.clear();
		if (m_font == NULL) return;
		if (m_textLayout) delete m_textLayout;

		std::wstring* drawText = &this->m_text;
		if (!m_passwordChar.empty()) {
			drawText = new std::wstring;
			int count = m_passwordChar.size() * m_text.size();
			for (size_t i = 0; i < m_text.size(); ++i)
			{
				*drawText += m_passwordChar;
			}
		}
		else {
			*drawText = m_text;
		}

		if (!m_multiLine) {//单行编辑框
			m_font->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			bool bAlignLeft = (int(this->TextAlign) & int(HAlign::Left));
			float width = bAlignLeft ? EZUI_FLOAT_MAX : Width();
			m_textLayout = new TextLayout(*drawText, *m_font, SizeF{ width,(float)Height() }, this->TextAlign);
			m_fontBox = m_textLayout->GetFontBox();
			if (m_fontBox.Width < this->Width()) {
				m_scrollX = 0;
			}
			if (!bAlignLeft && m_fontBox.Width > this->Width()) {
				ezui::TextAlign tmp = this->TextAlign;
				tmp = ezui::TextAlign((int)tmp & ~(int)HAlign::Center);
				tmp = ezui::TextAlign((int)tmp & ~(int)HAlign::Right);
				tmp = ezui::TextAlign((int)tmp | (int)HAlign::Left);
				m_textLayout->SetTextAlign(tmp);
			}
			if (m_fontBox.Width > this->Width() && m_scrollX + m_fontBox.Width < this->Width()) {
				m_scrollX = this->Width() - m_fontBox.Width;
			}
		}
		else {//多行编辑框
			m_font->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
			m_textLayout = new TextLayout(*drawText, *m_font, SizeF{ (float)Width(),EZUI_FLOAT_MAX }, TextAlign::TopLeft);
			m_fontBox = m_textLayout->GetFontBox();
		}
		if (drawText != &this->m_text) {
			delete drawText;
		}
		this->SetContentSize({ m_fontBox.Width , m_fontBox.Height });
		if (m_multiLine) {
			this->GetScrollBar()->RefreshScroll();
		}
		BuildCare();
	}

	void TextBox::BuildCare() {
		if (!m_textLayout) return;

		if (m_textPos < 0) {
			m_textPos = 0;
		}
		if (m_textPos > (int)m_text.size()) {
			m_textPos = m_text.size();
		}

		Point pt = m_textLayout->HitTestTextPosition(m_textPos, FALSE);
		m_careRect.X = pt.X;
		m_careRect.Y = pt.Y;
		m_careRect.Height = m_textLayout->GetFontHeight();
		m_careRect.Width = 1;

		if (!m_multiLine) {
			//使光标一直在输入框内
			int drawX = m_careRect.X + m_scrollX;
			if (drawX < 0) {//光标在最左侧
				m_scrollX -= drawX;
			}
			if (drawX > Width()) {//光标在最右侧
				int ofssetX = (Width() - drawX);
				m_scrollX += ofssetX;
			}
		}

	}
	void TextBox::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		m_lastX = 0;
		m_lastY = 0;
		auto mbtn = arg.Button;
		auto point = arg.Location;

		if (mbtn == MouseButton::Left) {
			m_down = true;
			m_point_Start = ConvertPoint(point);
			if (m_textLayout) {
				int fontHeight;
				m_selectRects.clear();
				m_pointA = m_textLayout->HitTestPoint(m_point_Start, &m_A_TextPos, &m_A_isTrailingHit, &fontHeight);
				m_careRect.X = m_pointA.X;
				m_careRect.Y = m_pointA.Y;
				m_careRect.Width = 1;
				m_careRect.Height = fontHeight;

				m_textPos = m_A_TextPos;
				if (m_A_isTrailingHit) {
					++m_textPos;
				}
			}
			Invalidate();
		}
	}

	void TextBox::OnMouseWheel(const MouseEventArgs& arg)
	{
		__super::OnMouseWheel(arg);
		if (!m_multiLine) {//单行
			int textWidth = m_fontBox.Width;
			if (arg.ZDelta > 0 && textWidth > Width()) {
				m_scrollX += std::abs(arg.ZDelta) * 0.5;
				if (m_scrollX > 0) {
					m_scrollX = 0;
				}
				Invalidate();
			}
			else if (arg.ZDelta<0 && textWidth>Width()) {
				m_scrollX -= std::abs(arg.ZDelta) * 0.5;
				if (-m_scrollX + Width() > textWidth) {
					m_scrollX = -(textWidth - Width());
				}
				Invalidate();
			}
		}
	}

	ScrollBar* TextBox::GetScrollBar()
	{
		return &m_vScrollbar;
	}
	void TextBox::Offset(int _sliderY) {
		this->m_scrollY = _sliderY;
		Invalidate();
	}

	void TextBox::OnLayout()
	{
		__super::OnLayout();
		m_scrollX = 0;
		m_scrollY = 0;
		m_selectRects.clear();
		if (!m_multiLine && Height() != m_lastHeight) {
			m_lastHeight = Height();
			Analysis();
		}
		if (m_multiLine && Width() != m_lastWidth) {
			m_lastWidth = Width();
			Analysis();
		}
		this->SetContentSize({ m_fontBox.Width ,m_multiLine ? m_fontBox.Height : Height() });
		this->GetScrollBar()->RefreshScroll();
		this->EndLayout();
	}

	Point TextBox::ConvertPoint(const Point& pt) {
		int _x = -m_scrollX;
		int _y = -m_scrollY;
		return Point{ pt.X + _x,pt.Y + _y };
	}

	void TextBox::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		auto point = arg.Location;
		if (m_down) {
			m_point_End = ConvertPoint(point);
			if (m_textLayout) {
				int fontHeight;
				m_selectRects.clear();//
				m_pointB = m_textLayout->HitTestPoint(m_point_End, &m_B_TextPos, &m_B_isTrailingHit, &fontHeight);

				BuildSelectedRect();

				if (!m_multiLine) {//单行
					//当鼠标往左侧移动
					int textWidth = m_fontBox.Width;
					if (m_lastX > point.X) {
						m_lastX = point.X;
						if (textWidth > Width() && m_scrollX < 0 && point.X < 0) {
							m_scrollX += 3;
							Invalidate();
							return;
						}
					}
					//当鼠标往右侧移动
					if (m_lastX < point.X) {
						m_lastX = point.X;
						if (textWidth > Width() && point.X > Width()) {
							m_scrollX -= 3;
							if (-m_scrollX + Width() > textWidth) {
								m_scrollX = -(textWidth - Width());
							}
							Invalidate();
							return;
						}
					}
				}
				Invalidate();
			}
		}
	}
	void TextBox::OnMouseUp(const MouseEventArgs& arg)
	{
		__super::OnMouseUp(arg);
		m_down = false;
		m_lastX = 0;
		m_lastY = 0;
		Invalidate();
	}
	void TextBox::OnFocus(const FocusEventArgs& arg)
	{
		__super::OnFocus(arg);
		m_focus = true;
		m_bCareShow = true;
		m_timer->Start();
		Invalidate();
	}
	void TextBox::OnKillFocus(const KillFocusEventArgs& arg)
	{
		__super::OnKillFocus(arg);
		m_down = false;
		m_focus = false;
		m_bCareShow = false;
		m_timer->Stop();
		this->Invalidate();
	}
	const UIString TextBox::GetText()
	{
		return UIString(this->m_text);
	}
	void TextBox::SetText(const UIString& text)
	{
		this->m_text = text.unicode();
		Analysis();
	}
	bool TextBox::IsMultiLine()
	{
		return m_multiLine;
	}
	void TextBox::SetMultiLine(bool multiLine)
	{
		if (this->m_multiLine != multiLine) {
			this->m_multiLine = multiLine;
			Analysis();
		}
	}
	void TextBox::SetReadOnly(bool bReadOnly)
	{
		this->m_readOnly = bReadOnly;
	}
	bool TextBox::IsReadOnly()
	{
		return this->m_readOnly;
	}
	void TextBox::SetMaxLength(int maxLen)
	{
		this->m_maxLen = maxLen;
	}
	void TextBox::SetPlaceholderText(const UIString& text)
	{
		this->m_placeholder = text.unicode();
	}
	void TextBox::SetPasswordChar(const UIString& passwordChar)
	{
		this->m_passwordChar = passwordChar.unicode();
	}
	Rect TextBox::GetCareRect()
	{
		Rect rect(m_careRect);
		rect.X += m_scrollX;//偏移
		rect.Y += m_scrollY;
		return rect;
	}
	void TextBox::Insert(const UIString& str)
	{
		InsertUnicode(str.unicode());
		Analysis();//分析字符串
	}
	void TextBox::SetAttribute(const UIString& key, const UIString& value) {
		__super::SetAttribute(key, value);
		do
		{
			if (key == "valign") {
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)VAlign::Top);
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)VAlign::Mid);
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)VAlign::Bottom);
				VAlign v = VAlign::Mid;
				if (value == "top") {
					v = VAlign::Top;
				}
				else if (value == "bottom") {
					v = VAlign::Bottom;
				}
				this->TextAlign = ezui::TextAlign((int)this->TextAlign | (int)v);
				break;
			}
			if (key == "halign") {
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)HAlign::Left);
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)HAlign::Center);
				this->TextAlign = ezui::TextAlign((int)this->TextAlign & ~(int)HAlign::Right);
				HAlign h = HAlign::Center;
				if (value == "left") {
					h = HAlign::Left;
				}
				else if (value == "right") {
					h = HAlign::Right;
				}
				this->TextAlign = ezui::TextAlign((int)this->TextAlign | (int)h);
				break;
			}
			if (key == "passwordchar") {
				this->SetPasswordChar(value);
				break;
			}
			if (key == "placeholder") {
				this->SetPlaceholderText(value);
				break;
			}
			if (key == "text" || key == "value") {
				this->SetText(value);
				break;
			}
			if (key == "readonly") {
				if (value == "true") {
					this->SetReadOnly(true);
					break;
				}
				if (value == "false") {
					this->SetReadOnly(false);
					break;
				}
			}
			if (key == "multiline") {
				if (value == "true") {
					this->m_multiLine = true;
					break;
				}
				if (value == "false") {
					this->m_multiLine = false;
					break;
				}
			}
		} while (false);
	}

	void TextBox::OnForePaint(PaintEventArgs& e) {
		std::wstring fontFamily = GetFontFamily();
		auto fontSize = GetFontSize();
		if (fontSize == 0)return;
		if (m_font == NULL || ((m_font != NULL) && (m_font->GetFontFamily() != fontFamily || m_font->GetFontSize() != fontSize))) {
			if (m_font != NULL) {
				delete m_font;
			}
			m_font = new Font(fontFamily, fontSize);
			Analysis();
		}
		const Color& fontColor = GetForeColor();
		e.Graphics.SetFont(fontFamily, fontSize);
		if (m_text.empty()) {
			Color placeholderColor = fontColor;
			placeholderColor.SetA(fontColor.GetA() * 0.6);
			e.Graphics.SetColor(placeholderColor);
			e.Graphics.DrawString(m_placeholder, RectF(0, 0, (float)Width(), (float)Height()), m_multiLine ? TextAlign::TopLeft : this->TextAlign);
		}

		if (m_selectRects.size() > 0) {
			Color selectedColor = fontColor;
			selectedColor.SetA(fontColor.GetA() * 0.35);
			e.Graphics.SetColor(selectedColor);
			for (auto& it : m_selectRects) {
				if (!it.IsEmptyArea()) {
					RectF rect(it);
					rect.X += m_scrollX;//偏移
					rect.Y += m_scrollY;
					e.Graphics.FillRectangle(rect);
				}
			}
		}

		if (m_textLayout) {
			e.Graphics.SetColor(fontColor);
			e.Graphics.DrawTextLayout(*m_textLayout, PointF{ (float)m_scrollX, (float)m_scrollY });
		}

		if (!m_careRect.IsEmptyArea() && m_focus) {
			if (m_bCareShow) {
				RectF rect(m_careRect.X, m_careRect.Y, m_careRect.Width, m_careRect.Height);
				rect.X += m_scrollX;//偏移
				rect.Y += m_scrollY;
				if (ezui::IsFloatEqual(rect.X, 0.0f)) {
					//如果光标刚好在边框起始位置
					rect.X = (1 * this->GetScale());
				}
				else if (ezui::IsFloatEqual(rect.X, this->Width())) {
					//如果光标刚好在边框末尾
					rect.X = this->Width() - (1 * this->GetScale());
				}
				rect.Width = rect.Width * this->GetScale();
				e.Graphics.SetColor(fontColor);
				e.Graphics.FillRectangle(rect);
			}
		}
	}
}