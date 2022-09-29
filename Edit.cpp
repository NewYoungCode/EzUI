#include "Edit.h"
//#include "EzUI.h"
namespace EzUI {

	Edit::~Edit() {
		if (textFormat) delete textFormat;
		if (textLayout) delete textLayout;
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
					Analysis();
					BuildCare(pos - 1);
				}
				else {
					text.erase(text.size() - 1, 1);
					Analysis();
					BuildCare(text.size() - 1);
				}
			}
			Invalidate();
		}
		if (wParam < 32)return;//控制字符
		WCHAR&& _char = (WCHAR)wParam;
		text += _char;
		Analysis();
		BuildCare(text.size());
		Invalidate();
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
	}

	void Edit::BuildCare(int _TextPos) {
		if (textLayout) {
			Point pt = textLayout->HitTestTextPosition(_TextPos, TRUE);
			TextPos = _TextPos;
			careRect.X = pt.X;
			careRect.Y = pt.Y;
			careRect.Height = FontHeight;
			careRect.Width = 1;
		}
	}

	void Edit::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		if (mbtn == MouseButton::Left) {
			_focus = true;
			point_Start = point;
			if (textLayout) {
				selectRect = Rect();
				A = textLayout->HitTestPoint(point_Start, A_TextPos, A_isTrailingHit);
				careRect.X = A.X;
				careRect.Y = A.Y;
				careRect.Width = 1;
				careRect.Height = FontHeight;
			}
			Invalidate();
		}
	}
	void Edit::OnMouseMove(const Point& point)
	{
		if (_focus) {
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
		_focus = false;
		Invalidate();
		this;
	}
	void Edit::OnKillFocus()
	{
		__super::OnKillFocus();
		_focus = false;
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
	bool Edit::OnSize(const Size& sz) {
		if (__super::OnSize(sz)) {
			Analysis();
			return true;
		}
		return false;
	}

	void Edit::OnForePaint(PaintEventArgs& e) {
		if (textLayout) {
			e.Painter.DrawTextLayout({ 0,0 }, textLayout, GetForeColor());
		}
		if (!selectRect.IsEmptyArea()) {
			e.Painter.FillRectangle(selectRect, Color(100, 255, 0, 0));
		}
		if (!careRect.IsEmptyArea()) {
			e.Painter.FillRectangle(careRect, Color(255, 0, 0, 0));
		}

	}
}