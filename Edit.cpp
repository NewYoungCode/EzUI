#include "Edit.h"
namespace EzUI {
	Edit::~Edit() {
		timer.Stop();
		if (textFormat) delete textFormat;
		if (textLayout) delete textLayout;
	}
	Edit::Edit() {
		timer.Interval = 500;
		timer.Tick = [&]() {
			if (!careRect.IsEmptyArea() && _focus) {
				__i++;
				this->Invalidate();
			}
		};
	}
	void Edit::OnChar(WPARAM wParam, LPARAM lParam)
	{
		if (wParam == VK_BACK) { //退格键
			if (text.size() > 0) {
				if (!selectRect.IsEmptyArea()) {
					int pos;
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
						int count = std::abs(pos2 - pos1) + 1;
						text.erase(pos, count);
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
						int count = std::abs(pos2 - pos1) + 1;
						text.erase(pos, count);
					}

					isTrailingHit = FALSE;
					TextPos = pos;
					Analysis();

				}
				else {

					isTrailingHit = FALSE;
					TextPos--;
					if (TextPos > -1) {
						text.erase(TextPos, 1);
					}
					Analysis();

				}
			}
			Invalidate();
		}
		if (wParam < 32)return;//控制字符
		WCHAR&& _char = (WCHAR)wParam;

		WCHAR _buf[2]{ _char,0 };
		Insert(_buf);

		Analysis();
		Invalidate();
	}

	bool Edit::GetSelectedRange(int* outPos, int* outCount) {
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
	void Edit::Insert(const std::wstring& str) {
		if (TextPos < 0)TextPos = 0;
		if (TextPos > text.size()) {
			TextPos = text.size();
		}
		text.insert(TextPos, str);
		TextPos += str.size();
	}
	void Edit::Delete() {

		if (text.size() <= 0)return;
		
		int pos, count;
		if (GetSelectedRange(&pos, &count)) {//删除选中的
			isTrailingHit = FALSE;
			TextPos = pos;
			text.erase(pos, count);
		}
		else {//删除单个字符
			isTrailingHit = FALSE;
			TextPos--;
			if (TextPos > -1) {
				text.erase(TextPos, 1);
			}
		}

	}

	void Edit::OnKeyDown(WPARAM wParam)
	{
		__super::OnKeyDown(wParam);
		//Debug::Log(utf8("按下了%d"), wParam);
	}
	void Edit::Analysis()
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
		if (textFormat) delete textFormat;
		textFormat = new TextFormat(GetFontFamily().utf16(), GetFontSize(), TextAlign::MiddleLeft);
		if (textLayout) delete textLayout;
		textLayout = new TextLayout(text, { 16777216, Height() }, textFormat);
		FontHeight = textLayout->GetFontSize().Height;

		if (TextPos < 0) {
			TextPos = 0;
			isTrailingHit = FALSE;
		}
		Point pt = textLayout->HitTestTextPosition(TextPos, isTrailingHit);
		careRect.X = pt.X;
		careRect.Y = pt.Y;
		careRect.Height = FontHeight;
		careRect.Width = 1;
	}

	void Edit::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		_focus = true;
		Invalidate();
		__i = 0;
		timer.Start();

		if (mbtn == MouseButton::Left) {
			_down = true;
			point_Start = point;

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
	void Edit::OnMouseMove(const Point& point)
	{
		if (_down) {
			point_End = point;
			//Debug::Log("%d %d", point_End.X, point_End.Y);
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
				Invalidate();
			}
		}
	}
	void Edit::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		_down = false;
		Invalidate();
		this;
	}
	void Edit::OnKillFocus()
	{
		__super::OnKillFocus();
		_down = false;
		_focus = false;
		__i = 0;
		timer.Stop();
		this->Invalidate();
	}
	EString Edit::GetText()
	{
		return EString(text);
	}
	void Edit::SetText(const EString& _text)
	{
		text = _text.utf16();
		Analysis();
	}
	void Edit::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "placeholder") {
			Placeholder = value;
		}
	}
	bool Edit::OnSize(const Size& sz) {
		if (__super::OnSize(sz)) {
			Analysis();
			return true;
		}
		return false;
	}

	void Edit::OnForePaint(PaintEventArgs& e) {
		if (text.empty()) {
			Color c = GetForeColor();
			byte r = c.GetR() - 30;
			byte g = c.GetG() - 30;
			byte b = c.GetB() - 30;
			e.Painter.DrawString(Placeholder.utf16(), GetFontFamily().utf16(), GetFontSize(), Color(r, g, b), { 0,0,Width(),Height() }, TextAlign::MiddleLeft);
		}
		if (textLayout) {
			e.Painter.DrawTextLayout({ 0,0 }, textLayout, GetForeColor());
		}
		if (!selectRect.IsEmptyArea()) {
			e.Painter.FillRectangle(selectRect, Color(100, 255, 0, 0));
		}
		if (!careRect.IsEmptyArea() && _focus) {
			if (__i % 2 == 0) {
				e.Painter.FillRectangle(careRect, Color(255, 0, 0, 0));
			}
		}
	}
}