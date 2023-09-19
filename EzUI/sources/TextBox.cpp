#include "TextBox.h"
namespace EzUI {
	TextBox::TextBox() { Init(); }
	TextBox::~TextBox() {
		_timer.Stop();
		if (_textLayout) { delete _textLayout; }
		if (_font) { delete _font; }
	}
	void TextBox::Init()
	{
		this->GetScrollBar()->SetWidth(5);
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [=](int offset) {
			this->Offset(offset);
			};

		Style.Cursor = LoadCursor(Cursor::IBEAM);
		_timer.Interval = 500;
		_timer.Tick = [&](Windows::Timer*) {
			if (this->Enable == false || this->ReadOnly == true) {
				_careShow = false;
				return;
			}
			if (!_careRect.IsEmptyArea() && _focus) {
				_careShow = !_careShow;
				this->Invalidate();
			}
			};
	}
	void TextBox::OnRemove() {
		__super::OnRemove();
		_timer.Stop();
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
		//Debug::Log(utf8("按下了%d"), wParam);
		do
		{
			if (wParam == VK_BACK) { //退格键
				if (ReadOnly) {
					break;
				}
				OnBackspace();//退格键的操作在里面
				Analysis();//重新分析
				Invalidate();//刷新
				break;
			}
			if (wParam == 1) {
				SelectedAll();//全选
				Invalidate();//刷新
				break;
			}
			if (wParam == 3) {//复制
				Copy();
				break;
			}
			if (wParam == 24) {//ctrl+x裁剪
				if (ReadOnly) {
					break;
				}
				if (Copy()) {
					DeleteRange();//因为是剪切 所以要删除选中的这段
					Analysis();
					Invalidate();//刷新
				}
				break;
			}
			if (wParam == 22) {
				if (ReadOnly) {
					break;
				}
				Paste();//粘贴
				Analysis();//分析字符串
				Invalidate();//刷新
				break;
			}
			if (wParam == 26) {//ctrl+z撤销
				if (ReadOnly) {
					break;
				}
				break;
			}

		} while (false);

		if (wParam < 32)return;//控制字符
		if (ReadOnly) return;//只读
		WCHAR buf[2]{ (WCHAR)wParam ,0 };//
		_Insert(std::wstring(buf));//插入新的字符
		Analysis();//分析字符串
		Invalidate();//刷新
	}

	void TextBox::BuildSelectedRect() {
		_selectRects.clear();
		if (_textLayout) {
			Point point1, point2;
			if ((_A_TextPos + _A_isTrailingHit) < (_B_TextPos + _B_isTrailingHit)) {
				point1 = _textLayout->HitTestTextPosition(_A_TextPos, _A_isTrailingHit);
				point2 = _textLayout->HitTestTextPosition(_B_TextPos, _B_isTrailingHit);
			}
			else {
				point2 = _textLayout->HitTestTextPosition(_A_TextPos, _A_isTrailingHit);
				point1 = _textLayout->HitTestTextPosition(_B_TextPos, _B_isTrailingHit);
			}
			if (point1.Y != point2.Y) {//多行
				Rect rect1(point1.X, point1.Y, _fontBox.Width - point1.X, _textLayout->GetFontHeight());
				Rect rect2(0, point2.Y, point2.X, _textLayout->GetFontHeight());
				Rect rect3(0, rect1.GetBottom(), _fontBox.Width, rect2.GetTop() - rect1.GetBottom());
				_selectRects.push_back(rect1);
				_selectRects.push_back(rect3);
				_selectRects.push_back(rect2);
			}
			else {
				_selectRects.push_back(Rect(point1.X, point1.Y, point2.X - point1.X, _textLayout->GetFontHeight()));
			}
		}
	}

	bool TextBox::SelectedAll() {
		if (_textLayout && !_text.empty()) {
			_A = Point{ 0,0 };
			_A_isTrailingHit = FALSE;
			_A_TextPos = 0;

			_B = Point{ _fontBox.Width ,0 };
			_B_isTrailingHit = TRUE;
			_B_TextPos = _text.size() - 1;

			BuildSelectedRect();
			return true;
		}
		return false;
	}
	bool TextBox::GetSelectedRange(int* outPos, int* outCount) {
		if (_selectRects.size() > 0) {
			int pos, count;
			if ((_A_TextPos + _A_isTrailingHit) < (_B_TextPos + _B_isTrailingHit)) {
				int pos1 = _A_TextPos;
				if (_A_isTrailingHit == 1) {
					pos1 += 1;
				}
				int pos2 = _B_TextPos;
				if (_B_isTrailingHit == 0) {
					pos2 -= 1;
				}
				pos = pos1;
				count = std::abs(pos2 - pos1) + 1;
			}
			else {
				int pos1 = _A_TextPos;
				if (_A_isTrailingHit == 0) {
					pos1 -= 1;
				}
				int pos2 = _B_TextPos;
				if (_B_isTrailingHit == 1) {
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
	void TextBox::_Insert(const std::wstring& str) {
		DeleteRange();//先删除是否有选中的区域
		if (_textPos < 0)_textPos = 0;
		if (_textPos > (int)_text.size()) {
			_textPos = _text.size();
		}
		_text.insert(_textPos, str);
		_textPos += str.size();
		if (TextChanged) {
			TextChanged(EString(_text));
		}
	}
	bool TextBox::DeleteRange() {
		int pos, count;
		if (GetSelectedRange(&pos, &count)) {//删除选中的
			//isTrailingHit = FALSE;
			_textPos = pos;
			_text.erase(pos, count);
			if (TextChanged) {
				TextChanged(EString(_text));
			}
			return true;
		}
		return false;
	}
	bool TextBox::Copy() {
		int pos, count;
		if (!GetSelectedRange(&pos, &count))return false;
		std::wstring wBuf(_text.substr(pos, count));
		return EzUI::CopyToClipboard(wBuf, PublicData->HANDLE);
	}
	bool TextBox::Paste() {
		std::wstring wBuf;
		bool bRet = EzUI::GetClipboardData(&wBuf, PublicData->HANDLE);
		EString u8Str(wBuf);
		if (!_multiLine) {
			//行编辑框不允许有换行符
			EString::Replace(&u8Str, "\r", "");
			EString::Replace(&u8Str, "\n", "");
		}
		_Insert(u8Str.utf16());//插入新的字符
		return bRet;
	}
	void TextBox::OnBackspace() {
		if (_text.size() <= 0)return;

		if (!DeleteRange()) {//先看看有没有有选中的需要删除
			//否则删除单个字符
			_textPos--;
			if (_textPos > -1) {
				_text.erase(_textPos, 1);
				if (TextChanged) {
					TextChanged(EString(_text));
				}
			}
		}
	}
	void TextBox::OnKeyDown(const KeyboardEventArgs& arg)
	{
		__super::OnKeyDown(arg);
		WPARAM wParam = arg.wParam;
		LPARAM lParam = arg.lParam;
		/*	if (wParam == 16) {
				_down = true;
			}*/
		if (wParam == VK_LEFT) {
			_textPos--;
			_careShow = true;
			_selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
		if (wParam == VK_RIGHT) {
			_textPos++;
			_careShow = true;
			_selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
		//Debug::Log(utf8("按下了%d"), wParam);
	}

	void TextBox::Analysis()
	{
		_scrollX = 0;
		_scrollY = 0;
		_A = Point();
		_A_isTrailingHit = 0;
		_A_TextPos = 0;
		_B = Point();
		_B_isTrailingHit = 0;
		_B_TextPos = 0;
		_careRect = Rect();
		_selectRects.clear();
		if (_font == NULL) return;
		if (_textLayout) delete _textLayout;

		std::wstring* drawText = &this->_text;
		if (!PasswordChar.empty()) {
			drawText = new std::wstring;
			int count = PasswordChar.size() * _text.size();
			for (size_t i = 0; i < _text.size(); i++)
			{
				*drawText += PasswordChar;
			}
		}
		else {
			*drawText = _text;
		}

		if (!_multiLine) {//单行编辑框
			_font->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			_textLayout = new TextLayout(*drawText, *_font, Size{ __MAXFLOAT,Height() }, TextAlign::MiddleLeft);
			_fontBox = _textLayout->GetFontBox();
			if (_fontBox.Width < this->Width()) {
				_scrollX = 0;
			}
			if (_fontBox.Width > this->Width() && _scrollX + _fontBox.Width < this->Width()) {
				_scrollX = this->Width() - _fontBox.Width;
			}
		}
		else {//多行编辑框
			_font->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
			_textLayout = new TextLayout(*drawText, *_font, Size{ Width(),__MAXFLOAT }, TextAlign::TopLeft);
			_fontBox = _textLayout->GetFontBox();
		}
		if (drawText != &this->_text) {
			delete drawText;
		}
		this->SetContentSize({ _fontBox.Width , _fontBox.Height });
		if (_multiLine) {
			this->GetScrollBar()->RefreshScroll();
		}
		BuildCare();
	}

	void TextBox::BuildCare() {
		if (!_textLayout) return;

		if (_textPos < 0) {
			_textPos = 0;
		}
		if (_textPos > (int)_text.size()) {
			_textPos = _text.size();
		}

		Point pt = _textLayout->HitTestTextPosition(_textPos, FALSE);
		_careRect.X = pt.X;
		_careRect.Y = pt.Y;
		_careRect.Height = _textLayout->GetFontHeight();
		_careRect.Width = 1;

		if (!_multiLine) {
			//使光标一直在输入框内
			int drawX = _careRect.X + _scrollX;
			if (drawX < 0) {//光标在最左侧
				_scrollX -= drawX;
			}
			if (drawX > Width()) {//光标在最右侧
				int ofssetX = (Width() - drawX);
				_scrollX += ofssetX;
			}
		}

	}
	void TextBox::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		_focus = true;
		_lastX = 0;
		_lastY = 0;
		_careShow = true;
		_timer.Start();

		auto mbtn = arg.Button;
		auto point = arg.Location;

		if (mbtn == MouseButton::Left) {
			_down = true;
			_point_Start = ConvertPoint(point);
			if (_textLayout) {
				int fontHeight;
				_selectRects.clear();
				_A = _textLayout->HitTestPoint(_point_Start, &_A_TextPos, &_A_isTrailingHit, &fontHeight);
				_careRect.X = _A.X;
				_careRect.Y = _A.Y;
				_careRect.Width = 1;
				_careRect.Height = fontHeight;

				_textPos = _A_TextPos;
				if (_A_isTrailingHit) {
					_textPos++;
				}
			}
			Invalidate();
		}
	}

	void TextBox::OnMouseWheel(const MouseEventArgs& arg)
	{
		__super::OnMouseWheel(arg);
		if (!_multiLine) {//单行
			int textWidth = _fontBox.Width;
			if (arg.ZDelta > 0 && textWidth > Width()) {
				_scrollX += std::abs(arg.ZDelta) * 0.5;
				if (_scrollX > 0) {
					_scrollX = 0;
				}
				Invalidate();
			}
			else if (arg.ZDelta<0 && textWidth>Width()) {
				_scrollX -= std::abs(arg.ZDelta) * 0.5;
				if (-_scrollX + Width() > textWidth) {
					_scrollX = -(textWidth - Width());
				}
				Invalidate();
			}
		}
	}

	ScrollBar* TextBox::GetScrollBar()
	{
		return &_vScrollbar;
	}
	void TextBox::Offset(int _sliderY) {
		this->_scrollY = _sliderY;
		Invalidate();
	}

	void TextBox::OnLayout()
	{
		__super::OnLayout();
		_scrollX = 0;
		_scrollY = 0;
		_selectRects.clear();
		if (!_multiLine && Height() != _lastHeight) {
			_lastHeight = Height();
			Analysis();
		}
		if (_multiLine && Width() != _lastWidth) {
			_lastWidth = Width();
			Analysis();
		}
		this->SetContentSize({ _fontBox.Width ,_multiLine ? _fontBox.Height : Height() });
		this->GetScrollBar()->RefreshScroll();
		this->EndLayout();
	}

	Point TextBox::ConvertPoint(const Point& pt) {
		int _x = -_scrollX;
		int _y = -_scrollY;
		return Point{ pt.X + _x,pt.Y + _y };
	}

	void TextBox::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		auto point = arg.Location;
		if (_down) {
			_point_End = ConvertPoint(point);
			if (_textLayout) {
				int fontHeight;
				_selectRects.clear();//
				_B = _textLayout->HitTestPoint(_point_End, &_B_TextPos, &_B_isTrailingHit, &fontHeight);

				BuildSelectedRect();

				if (!_multiLine) {//单行
					//当鼠标往左侧移动
					int textWidth = _fontBox.Width;
					if (_lastX > point.X) {
						_lastX = point.X;
						if (textWidth > Width() && _scrollX < 0 && point.X < 0) {
							_scrollX += 3;
							Invalidate();
							return;
						}
					}
					//当鼠标往右侧移动
					if (_lastX < point.X) {
						_lastX = point.X;
						if (textWidth > Width() && point.X > Width()) {
							_scrollX -= 3;
							if (-_scrollX + Width() > textWidth) {
								_scrollX = -(textWidth - Width());
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
		_down = false;
		_lastX = 0;
		_lastY = 0;
		Invalidate();
	}
	void TextBox::OnKillFocus(const KillFocusEventArgs& arg)
	{
		__super::OnKillFocus(arg);
		_down = false;
		_focus = false;
		_careShow = false;
		_timer.Stop();
		this->Invalidate();
	}
	const EString TextBox::GetText()
	{
		return EString(this->_text);
	}
	void TextBox::SetText(const EString& text)
	{
		this->_text = text.utf16();
		Analysis();
	}
	bool TextBox::IsMultiLine()
	{
		return _multiLine;
	}
	void TextBox::SetMultiLine(bool multiLine)
	{
		if (this->_multiLine != multiLine) {
			this->_multiLine = multiLine;
			Analysis();
		}
	}
	Rect TextBox::GetCareRect()
	{
		Rect rect(_careRect);
		rect.X += _scrollX;//偏移
		rect.Y += _scrollY;
		return rect;
	}
	void TextBox::Insert(const EString& str)
	{
		_Insert(str.utf16());
		Analysis();//分析字符串
	}
	void TextBox::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		do
		{
			if (key == "passwordchar") {
				PasswordChar = value.utf16();
				break;
			}
			if (key == "placeholder") {
				this->Placeholder = value;
				break;
			}
			if (key == "text") {
				this->SetText(value);
				break;
			}
			if (key == "readonly") {
				if (value == "true") {
					this->ReadOnly = true;
					break;
				}
				if (value == "false") {
					this->ReadOnly = false;
					break;
				}
			}
			if (key == "multiline") {
				if (value == "true") {
					this->_multiLine = true;
					break;
				}
				if (value == "false") {
					this->_multiLine = false;
					break;
				}
			}
		} while (false);
	}

	void TextBox::OnForePaint(PaintEventArgs& e) {
		std::wstring fontFamily = GetFontFamily();
		auto fontSize = GetFontSize();
		if (fontSize == 0)return;
		if (_font == NULL || ((_font != NULL) && (_font->GetFontFamily() != fontFamily || _font->GetFontSize() != fontSize))) {
			if (_font != NULL) {
				delete _font;
			}
			_font = new Font(fontFamily, fontSize);
			Analysis();
		}
		const Color& fontColor = GetForeColor();
		e.Graphics.SetFont(fontFamily, fontSize);
		if (_text.empty()) {
			Color placeholderColor = fontColor;
			placeholderColor.SetA(fontColor.GetA() * 0.6);
			e.Graphics.SetColor(placeholderColor);
			e.Graphics.DrawString(Placeholder.utf16(), Rect(0, 0, Width(), Height()), _multiLine ? TextAlign::TopLeft : TextAlign::MiddleLeft);
		}

		if (_selectRects.size() > 0) {
			Color selectedColor = fontColor;
			selectedColor.SetA(fontColor.GetA() * 0.35);
			e.Graphics.SetColor(selectedColor);
			for (auto& it : _selectRects) {
				if (!it.IsEmptyArea()) {
					Rect rect(it);
					rect.X += _scrollX;//偏移
					rect.Y += _scrollY;
					e.Graphics.FillRectangle(rect);
				}
			}
		}

		if (_textLayout) {
			e.Graphics.SetColor(fontColor);
			e.Graphics.DrawTextLayout(*_textLayout, { _scrollX, _scrollY });
		}

		if (!_careRect.IsEmptyArea() && _focus) {
			if (_careShow) {
				Rect rect(_careRect);
				rect.X += _scrollX;//偏移
				rect.Y += _scrollY;
				if (rect.X == this->Width()) {//如果刚好处于边界
					rect.X = this->Width() - 1;
				}
				e.Graphics.SetColor(fontColor);
				e.Graphics.FillRectangle(rect);
			}
		}
	}
}