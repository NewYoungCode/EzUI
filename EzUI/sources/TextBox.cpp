#include "TextBox.h"
namespace EzUI {
#define FontHeight  FontBox.Height
	TextBox::~TextBox() {
		timer.Stop();
		if (textLayout) delete textLayout;
	}

	void TextBox::OnRemove() {
		timer.Stop();
		__super::OnRemove();
	}

	TextBox::TextBox() {
		Init();
	}
	TextBox::TextBox(Control* parent) :Control(parent)
	{
		Init();
	}
	void TextBox::OnKeyChar(WPARAM wParam, LPARAM lParam)
	{
		//Debug::Log(utf8("������%d"), wParam);
		do
		{
			if (wParam == VK_BACK) { //�˸��
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
				if (Copy()) {
					DeleteRange();//��Ϊ�Ǽ��� ����Ҫɾ��ѡ�е����
					Analysis();
					Invalidate();//ˢ��
				}
				break;
			}
			if (wParam == 26) {//ctrl+z����
				break;
			}
			if (wParam == 22) {
				Paste();//ճ��
				Analysis();//�����ַ���
				Invalidate();//ˢ��
				break;
			}
		} while (false);

		if (wParam < 32)return;//�����ַ�


		DeleteRange();//��ɾ���Ƿ���ѡ�е�����
		WCHAR buf[2]{ (WCHAR)wParam ,0 };
		Insert(buf);//�����µ��ַ�
		Analysis();//�����ַ���
		Invalidate();//ˢ��
	}

	bool TextBox::SelectedAll() {
		if (textLayout && !text.empty()) {
			A = Point{ 0,0 };
			A_isTrailingHit = FALSE;
			A_TextPos = 0;

			B = Point{ FontBox.Width ,0 };
			B_isTrailingHit = TRUE;
			B_TextPos = text.size() - 1;

			selectRect.X = 0;
			selectRect.Y = (this->Height() - FontHeight) / 2;
			selectRect.Width = FontBox.Width;
			selectRect.Height = FontBox.Height;
			return true;
		}
		return false;
	}

	bool TextBox::GetSelectedRange(int* outPos, int* outCount) {
		if (!selectRect.IsEmptyArea()) {
			int pos, count;
			if (A.X < B.X) {
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
			EString::Replace(&buf, "\r", "");//�б༭�������л��з�
			EString::Replace(&buf, "\n", "");//�б༭�������л��з�
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

	void TextBox::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		__super::OnKeyDown(wParam, lParam);
		/*	if (wParam == 16) {
				_down = true;
			}*/
		if (wParam == VK_LEFT) {
			TextPos--;
			_careShow = true;
			selectRect = Rect();
			BuildCare();
			Invalidate();
			return;
		}
		if (wParam == VK_RIGHT) {
			TextPos++;
			_careShow = true;
			selectRect = Rect();
			BuildCare();
			Invalidate();
			return;
		}
		//Debug::Log(utf8("������%d"), wParam);
	}

	void TextBox::OnKeyUp(WPARAM wParam, LPARAM lParam) {
		__super::OnKeyUp(wParam, lParam);
	}
	void TextBox::Init()
	{
		Cursor = Cursor::IBEAM;
		timer.Interval = 500;
		timer.Tick = [&](Windows::Timer*) {
			if (!careRect.IsEmptyArea() && _focus) {
				_careShow = !_careShow;
				this->Invalidate();
			}
		};
	}
	void TextBox::Analysis()
	{
		A = Point();
		A_isTrailingHit = 0;
		A_TextPos = 0;
		B = Point();
		B_isTrailingHit = 0;
		B_TextPos = 0;
		selectRect = Rect();
		careRect = Rect();
		if (GetFontFamily().empty() || GetFontSize() == 0 || GetRect().IsEmptyArea()) return;
		Font font(GetFontFamily().utf16(), GetFontSize());
		if (textLayout) delete textLayout;
		textLayout = new TextLayout(text, font);

		FontBox = textLayout->GetFontBox();

		if (FontBox.Width < this->Width()) {
			x = 0;
		}
		if (FontBox.Width > this->Width() && x + FontBox.Width < this->Width()) {
			x = this->Width() - FontBox.Width;
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
		careRect.Height = FontHeight;
		careRect.Width = 1;

		//ʹ���һֱ���������
		int drawX = careRect.X + x;
		if (drawX < 0) {//����������
			x -= drawX;
		}
		if (drawX > Width()) {//��������Ҳ�
			int ofssetX = (Width() - drawX);
			x += ofssetX;
		}
	}

	void TextBox::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		_focus = true;
		lastX = 0;
		Invalidate();
		_careShow = true;
		timer.Start();

		if (mbtn == MouseButton::Left) {
			_down = true;
			point_Start = ConvertPoint(point);

			if (textLayout == NULL) {
				Analysis();
			}
			if (textLayout) {
				selectRect = Rect();
				A = textLayout->HitTestPoint(point_Start, A_TextPos, A_isTrailingHit);
				careRect.X = A.X;
				careRect.Y = A.Y;
				careRect.Width = 1;
				careRect.Height = FontHeight;

				TextPos = A_TextPos;
				if (A_isTrailingHit) {
					TextPos++;
				}
			}
			Invalidate();
		}
	}
	void TextBox::OnMouseWheel(short zDelta, const Point& point) {
		__super::OnMouseWheel(zDelta, point);
	}

	Point TextBox::ConvertPoint(const Point& pt) {
		int _x = -x;
		return Point{ pt.X + _x,pt.X };
	}

	void TextBox::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (_down) {
			point_End = ConvertPoint(point);
			if (textLayout) {
				selectRect = Rect();
				B = textLayout->HitTestPoint(point_End, B_TextPos, B_isTrailingHit);
				Rect& rect = selectRect;
				rect.X = A.X;
				rect.Y = A.Y;
				rect.Height = FontHeight;
				rect.Width = B.X - A.X;
				if (rect.Width < 0) {
					rect.X = B.X;
					rect.Y = B.Y;
					rect.Width = -rect.Width;
				}

				//�����������ƶ�
				int textWidth = textLayout->GetFontBox().Width;
				if (lastX > point.X) {
					lastX = point.X;
					if (textWidth > Width() && x < 0 && point.X < 0) {
						x += 3;
						Invalidate();
						return;
					}
				}
				//��������Ҳ��ƶ�
				if (lastX < point.X) {
					lastX = point.X;
					if (textWidth > Width() && point.X > Width()) {
						x -= 3;
						if (-x + Width() > textWidth) {
							x = -(textWidth - Width());
						}
						Invalidate();
						return;
					}
				}
				Invalidate();
			}
		}
	}
	void TextBox::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		__super::OnMouseUp(mbtn, point);
		_down = false;
		lastX = 0;
		Invalidate();
		this;
	}
	void TextBox::OnKillFocus()
	{
		__super::OnKillFocus();
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
	Rect TextBox::GetCareRect()
	{
		Rect rect(careRect);
		rect.X += x;//ƫ��
		return rect;
	}
	void TextBox::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "placeholder") {
			Placeholder = value;
		}
		if (key == "text") {
			SetText(value);
		}
	}
	void TextBox::OnSize(const Size& sz) {
		__super::OnSize(sz);
		Analysis();
	}

	void TextBox::OnForePaint(PaintEventArgs& e) {

		Color fontColor = GetForeColor();

		e.Graphics.SetFont(GetFontFamily().utf16(), GetFontSize());

		if (text.empty()) {
			byte r = fontColor.GetR() - 20;
			byte g = fontColor.GetG() - 20;
			byte b = fontColor.GetB() - 20;
			e.Graphics.SetColor(Color(r, g, b));
			e.Graphics.DrawString(Placeholder.utf16(), Rect(0, 0, Width(), Height()), TextAlign::MiddleLeft);
		}

		e.Graphics.SetColor(fontColor);
		int y = 0;
		if (textLayout) {
			Size fontBox = textLayout->GetFontBox();
			y = (Height() - fontBox.Height) / 2;
			e.Graphics.DrawString(*textLayout, { x, y });
		}
		if (!selectRect.IsEmptyArea()) {
			Rect rect(selectRect);
			rect.X += x;//ƫ��
			rect.Y = y;

			e.Graphics.SetColor(SelectColor);
			e.Graphics.FillRectangle(rect);
		}
		if (!careRect.IsEmptyArea() && _focus) {
			if (_careShow) {
				Rect rect(careRect);
				rect.X += x;//ƫ��
				rect.Y = y;
				if (rect.X == this->Width()) {//����պô��ڱ߽�
					rect.X = this->Width() - 1;
				}

				e.Graphics.SetColor(fontColor);
				e.Graphics.FillRectangle(rect);
			}
		}
	}
}