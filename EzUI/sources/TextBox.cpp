#include "TextBox.h"
namespace EzUI {
	TextBox::TextBox() { Init(); }
	TextBox::~TextBox() {
		timer.Stop();
		if (textLayout) { delete textLayout; }
		if (font) { delete font; }
	}
	void TextBox::Init()
	{
		this->GetScrollBar()->SetWidth(5);
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OWner = this;
		this->GetScrollBar()->OffsetCallback = [=](int offset) {
			this->Offset(offset);
		};

		Style.Cursor = LoadCursor(Cursor::IBEAM);
		timer.Interval = 500;
		timer.Tick = [&](Windows::Timer*) {
			if (this->Enable == false || this->ReadOnly == true) {
				_careShow = false;
				return;
			}
			if (!careRect.IsEmptyArea() && _focus) {
				_careShow = !_careShow;
				this->Invalidate();
			}
		};
	}
	void TextBox::OnRemove() {
		__super::OnRemove();
		timer.Stop();
	}
	void TextBox::SetAutoWidth(bool flag)
	{
		//��Ҫ����
	}
	void TextBox::SetAutoHeight(bool flag)
	{
		//��Ҫ����
	}
	void TextBox::OnKeyChar(const KeyboardEventArgs& arg)
	{
		__super::OnKeyChar(arg);
		WPARAM wParam = arg.wParam;
		LPARAM lParam = arg.lParam;
		//Debug::Log(utf8("������%d"), wParam);
		do
		{
			if (wParam == VK_BACK) { //�˸��
				if (ReadOnly) {
					break;
				}
				OnBackspace();//�˸���Ĳ���������
				Analysis();//���·���
				Invalidate();//ˢ��
				break;
			}
			if (wParam == 1) {
				SelectedAll();//ȫѡ
				Invalidate();//ˢ��
				break;
			}
			if (wParam == 3) {//����
				Copy();
				break;
			}
			if (wParam == 24) {//ctrl+x�ü�
				if (ReadOnly) {
					break;
				}
				if (Copy()) {
					DeleteRange();//��Ϊ�Ǽ��� ����Ҫɾ��ѡ�е����
					Analysis();
					Invalidate();//ˢ��
				}
				break;
			}
			if (wParam == 22) {
				if (ReadOnly) {
					break;
				}
				Paste();//ճ��
				Analysis();//�����ַ���
				Invalidate();//ˢ��
				break;
			}
			if (wParam == 26) {//ctrl+z����
				if (ReadOnly) {
					break;
				}
				break;
			}

		} while (false);

		if (wParam < 32)return;//�����ַ�
		if (ReadOnly) return;//ֻ��
		DeleteRange();//��ɾ���Ƿ���ѡ�е�����
		WCHAR buf[2]{ (WCHAR)wParam ,0 };//
		Insert(buf);//�����µ��ַ�
		Analysis();//�����ַ���
		Invalidate();//ˢ��
	}

	void TextBox::BuildSelectedRect() {
		selectRects.clear();
		if (textLayout) {
			Point point1, point2;
			if ((A_TextPos + A_isTrailingHit) < (B_TextPos + B_isTrailingHit)) {
				point1 = textLayout->HitTestTextPosition(A_TextPos, A_isTrailingHit);
				point2 = textLayout->HitTestTextPosition(B_TextPos, B_isTrailingHit);
			}
			else {
				point2 = textLayout->HitTestTextPosition(A_TextPos, A_isTrailingHit);
				point1 = textLayout->HitTestTextPosition(B_TextPos, B_isTrailingHit);
			}
			if (point1.Y != point2.Y) {//����
				Rect rect1(point1.X, point1.Y, _fontBox.Width - point1.X, textLayout->GetFontHeight());
				Rect rect2(0, point2.Y, point2.X, textLayout->GetFontHeight());
				Rect rect3(0, rect1.GetBottom(), _fontBox.Width, rect2.GetTop() - rect1.GetBottom());
				selectRects.push_back(rect1);
				selectRects.push_back(rect3);
				selectRects.push_back(rect2);
			}
			else {
				selectRects.push_back(Rect(point1.X, point1.Y, point2.X - point1.X, textLayout->GetFontHeight()));
			}
		}
	}

	bool TextBox::SelectedAll() {
		if (textLayout && !text.empty()) {
			A = Point{ 0,0 };
			A_isTrailingHit = FALSE;
			A_TextPos = 0;

			B = Point{ _fontBox.Width ,0 };
			B_isTrailingHit = TRUE;
			B_TextPos = text.size() - 1;

			BuildSelectedRect();
			return true;
		}
		return false;
	}
	bool TextBox::GetSelectedRange(int* outPos, int* outCount) {
		if (selectRects.size() > 0) {
			int pos, count;
			if ((A_TextPos + A_isTrailingHit) < (B_TextPos + B_isTrailingHit)) {
				int pos1 = A_TextPos;
				if (A_isTrailingHit == 1) {
					pos1 += 1;
				}
				int pos2 = B_TextPos;
				if (B_isTrailingHit == 0) {
					pos2 -= 1;
				}
				pos = pos1;
				count = std::abs(pos2 - pos1) + 1;
			}
			else {
				int pos1 = A_TextPos;
				if (A_isTrailingHit == 0) {
					pos1 -= 1;
				}
				int pos2 = B_TextPos;
				if (B_isTrailingHit == 1) {
					pos2 += 1;
				}
				pos = pos2;
				count = std::abs(pos2 - pos1) + 1;
			}
			*outPos = pos;
			*outCount = count;
			if (outCount > 0) {
				return true;
			}
		}
		return false;
	}
	void TextBox::Insert(const std::wstring& str) {
		if (TextPos < 0)TextPos = 0;
		if (TextPos > (int)text.size()) {
			TextPos = text.size();
		}
		text.insert(TextPos, str);
		TextPos += str.size();
		if (TextChange) {
			TextChange(EString(text));
		}
	}
	bool TextBox::DeleteRange() {
		int pos, count;
		if (GetSelectedRange(&pos, &count)) {//ɾ��ѡ�е�
			//isTrailingHit = FALSE;
			TextPos = pos;
			text.erase(pos, count);

			if (TextChange) {
				TextChange(EString(text));
			}
			return true;
		}
		return false;
	}
	bool TextBox::Copy() {
		do
		{
			int pos, count;
			if (!GetSelectedRange(&pos, &count))break;
			std::wstring wBuf(text.substr(pos, count));
			std::string str;
			EString::UnicodeToANSI(wBuf, &str);

			//�򿪼�����
			if (!OpenClipboard(PublicData->HANDLE))break;
			//��ռ�����
			EmptyClipboard();
			//Ϊ���а������ڴ�
			HGLOBAL clip = GlobalAlloc(GMEM_DDESHARE, (str.size() + 1));
			memcpy((void*)clip, str.c_str(), (str.size() + 1));
			//����
			GlobalUnlock(clip);
			SetClipboardData(CF_TEXT, clip);
			CloseClipboard();
			return true;
		} while (false);
		return false;
	}
	bool TextBox::Paste() {
		do
		{
			//ֻ�����ı�
			if (!IsClipboardFormatAvailable(CF_TEXT))break;
			//�򿪼�����
			if (!OpenClipboard(PublicData->HANDLE))break;
			//��ȡ����������
			HANDLE hClipboard = GetClipboardData(CF_TEXT);
			EString buf((CHAR*)GlobalLock(hClipboard));
			if (!multiLine) {
				EString::Replace(&buf, "\r", "");//�б༭�������л��з�
				EString::Replace(&buf, "\n", "");//�б༭�������л��з�
			}
			std::wstring wBuf;

			EString::ANSIToUniCode(buf, &wBuf);
			//����
			GlobalUnlock(hClipboard);
			CloseClipboard();

			DeleteRange();//��ɾ���Ƿ���ѡ�е�����
			Insert(wBuf);//�����µ��ַ�
			return true;

		} while (false);
		return false;
	}
	void TextBox::OnBackspace() {
		if (text.size() <= 0)return;

		if (!DeleteRange()) {//�ȿ�����û����ѡ�е���Ҫɾ��
			//����ɾ�������ַ�
			TextPos--;
			if (TextPos > -1) {
				text.erase(TextPos, 1);
				if (TextChange) {
					TextChange(EString(text));
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
			TextPos--;
			_careShow = true;
			selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
		if (wParam == VK_RIGHT) {
			TextPos++;
			_careShow = true;
			selectRects.clear();
			BuildCare();
			Invalidate();
			return;
		}
		//Debug::Log(utf8("������%d"), wParam);
	}

	void TextBox::Analysis()
	{
		scrollX = 0;
		scrollY = 0;
		A = Point();
		A_isTrailingHit = 0;
		A_TextPos = 0;
		B = Point();
		B_isTrailingHit = 0;
		B_TextPos = 0;
		careRect = Rect();
		selectRects.clear();
		if (font == NULL) return;
		if (textLayout) delete textLayout;

		std::wstring* drawText = &this->text;
		if (!PasswordChar.empty()) {
			drawText = new std::wstring;
			int count = PasswordChar.size() * text.size();
			for (size_t i = 0; i < text.size(); i++)
			{
				*drawText += PasswordChar;
			}
		}
		else {
			*drawText = text;
		}

		if (!multiLine) {//���б༭��
			textLayout = new TextLayout(*drawText, *font, Size{ __MAXFLOAT,Height() }, TextAlign::MiddleLeft);
			_fontBox = textLayout->GetFontBox();
			if (_fontBox.Width < this->Width()) {
				scrollX = 0;
			}
			if (_fontBox.Width > this->Width() && scrollX + _fontBox.Width < this->Width()) {
				scrollX = this->Width() - _fontBox.Width;
			}
		}
		else {//���б༭��
			textLayout = new TextLayout(*drawText, *font, Size{ Width(),__MAXFLOAT }, TextAlign::TopLeft);
			_fontBox = textLayout->GetFontBox();
		}
		if (drawText != &this->text) {
			delete drawText;
		}
		this->SetContentSize({ _fontBox.Width , _fontBox.Height });
		if (multiLine) {
			this->GetScrollBar()->RefreshScroll();
		}
		BuildCare();
	}

	void TextBox::BuildCare() {
		if (!textLayout) return;

		if (TextPos < 0) {
			TextPos = 0;
		}
		if (TextPos > (int)text.size()) {
			TextPos = text.size();
		}

		Point pt = textLayout->HitTestTextPosition(TextPos, FALSE);
		careRect.X = pt.X;
		careRect.Y = pt.Y;
		careRect.Height = textLayout->GetFontHeight();
		careRect.Width = 1;

		if (!multiLine) {
			//ʹ���һֱ���������
			int drawX = careRect.X + scrollX;
			if (drawX < 0) {//����������
				scrollX -= drawX;
			}
			if (drawX > Width()) {//��������Ҳ�
				int ofssetX = (Width() - drawX);
				scrollX += ofssetX;
			}
		}

	}
	void TextBox::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		_focus = true;
		lastX = 0;
		lastY = 0;
		_careShow = true;
		timer.Start();

		auto mbtn = arg.Button;
		auto point = arg.Location;

		if (mbtn == MouseButton::Left) {
			_down = true;
			point_Start = ConvertPoint(point);
			if (textLayout) {
				int fontHeight;
				selectRects.clear();
				A = textLayout->HitTestPoint(point_Start, &A_TextPos, &A_isTrailingHit, &fontHeight);
				careRect.X = A.X;
				careRect.Y = A.Y;
				careRect.Width = 1;
				careRect.Height = fontHeight;

				TextPos = A_TextPos;
				if (A_isTrailingHit) {
					TextPos++;
				}
			}
			Invalidate();
		}
	}

	ScrollBar* TextBox::GetScrollBar()
	{
		return &_vsb;
	}
	void TextBox::Offset(int _sliderY) {
		scrollY = _sliderY;
		Invalidate();
	}

	void TextBox::OnLayout()
	{
		__super::OnLayout();
		scrollX = 0;
		scrollY = 0;
		if (!multiLine && Height() != lastHeight) {
			lastHeight = Height();
			Analysis();
		}
		if (multiLine && Width() != lastWidth) {
			lastWidth = Width();
			Analysis();
		}
		this->SetContentSize({ _fontBox.Width ,_fontBox.Height });
		this->GetScrollBar()->RefreshScroll();
		this->EndLayout();
	}

	Point TextBox::ConvertPoint(const Point& pt) {
		int _x = -scrollX;
		int _y = -scrollY;
		return Point{ pt.X + _x,pt.Y + _y };
	}

	void TextBox::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		auto point = arg.Location;
		if (_down) {
			point_End = ConvertPoint(point);
			if (textLayout) {
				int fontHeight;
				selectRects.clear();//
				B = textLayout->HitTestPoint(point_End, &B_TextPos, &B_isTrailingHit, &fontHeight);

				BuildSelectedRect();

				if (!multiLine) {//����
					//�����������ƶ�
					int textWidth = _fontBox.Width;
					if (lastX > point.X) {
						lastX = point.X;
						if (textWidth > Width() && scrollX < 0 && point.X < 0) {
							scrollX += 3;
							Invalidate();
							return;
						}
					}
					//��������Ҳ��ƶ�
					if (lastX < point.X) {
						lastX = point.X;
						if (textWidth > Width() && point.X > Width()) {
							scrollX -= 3;
							if (-scrollX + Width() > textWidth) {
								scrollX = -(textWidth - Width());
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
		lastX = 0;
		lastY = 0;
		Invalidate();
		this;
	}
	void TextBox::OnKillFocus(const KillFocusEventArgs& arg)
	{
		__super::OnKillFocus(arg);
		_down = false;
		_focus = false;
		_careShow = false;
		timer.Stop();
		this->Invalidate();
	}
	const EString TextBox::GetText()
	{
		return EString(text);
	}
	void TextBox::SetText(const EString& _text)
	{
		text = _text.utf16();
		Analysis();
	}
	bool TextBox::IsMultiLine()
	{
		return multiLine;
	}
	void TextBox::SetMultiLine(bool _multiLine)
	{
		if (multiLine != _multiLine) {
			multiLine = _multiLine;
			Analysis();
		}
	}
	Rect TextBox::GetCareRect()
	{
		Rect rect(careRect);
		rect.X += scrollX;//ƫ��
		rect.Y += scrollY;
		return rect;
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
					this->multiLine = true;
					break;
				}
				if (value == "false") {
					this->multiLine = false;
					break;
				}
			}
		} while (false);
	}

	void TextBox::OnForePaint(PaintEventArgs& e) {
		std::wstring fontFamily = GetFontFamily();
		const int& fontSize = GetFontSize();

		if (font == NULL || ((font != NULL) && (font->GetFontFamily() != fontFamily || font->GetFontSize() != fontSize))) {
			if (font != NULL) {
				delete font;
			}
			font = new Font(fontFamily, fontSize);
			Analysis();
		}
		const Color& fontColor = GetForeColor();
		e.Graphics.SetFont(fontFamily, fontSize);
		if (text.empty()) {
			Color placeholderColor = fontColor;
			placeholderColor.SetA(fontColor.GetA() * 0.5);
			e.Graphics.SetColor(placeholderColor);
			e.Graphics.DrawString(Placeholder.utf16(), Rect(0, 0, Width(), Height()), multiLine ? TextAlign::TopLeft : TextAlign::MiddleLeft);
		}

		if (selectRects.size() > 0) {
			Color selectedColor = fontColor;
			selectedColor.SetA(fontColor.GetA() * 0.35);
			e.Graphics.SetColor(selectedColor);
			for (auto& it : selectRects) {
				if (!it.IsEmptyArea()) {
					Rect rect(it);
					rect.X += scrollX;//ƫ��
					rect.Y += scrollY;
					e.Graphics.FillRectangle(rect);
				}
			}
		}

		if (textLayout) {
			e.Graphics.SetColor(fontColor);
			e.Graphics.DrawString(*textLayout, { scrollX, scrollY });
		}

		if (!careRect.IsEmptyArea() && _focus) {
			if (_careShow) {
				Rect rect(careRect);
				rect.X += scrollX;//ƫ��
				rect.Y += scrollY;
				if (rect.X == this->Width()) {//����պô��ڱ߽�
					rect.X = this->Width() - 1;
				}
				e.Graphics.SetColor(fontColor);
				e.Graphics.FillRectangle(rect);
			}
		}
	}
}