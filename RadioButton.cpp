#include "RadioButton.h"

namespace EzUI {

	RadioButton::RadioButton()
	{
		//Cursor = IDC_HAND;
		TextAlign = TextAlign::MiddleLeft;
	}

	RadioButton::~RadioButton()
	{
	}

	void RadioButton::SetCheck(bool checked)
	{
		_checked = checked;
		Invalidate();
	}

	bool RadioButton::GetCheck()
	{
		return _checked;
	}

	void RadioButton::OnMouseClick(MouseButton btn, const Point& pt)
	{
		SetCheck(true);
		for (auto& it : Parent->GetControls()) {
			RadioButton* rbtn = dynamic_cast<RadioButton*>(it);
			if (rbtn && rbtn != this && rbtn->GetCheck() == true) {
				rbtn->SetCheck(false);
			}
		}
	}

	void RadioButton::OnForePaint(PaintEventArgs& args)
	{
		int fontHeight = Height();
		if (!_text.empty()) {
			RectF box;
			args.Painter.MeasureString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), box);
			fontHeight = (int)box.Height - 2;
			int x = fontHeight + Indent;
			int cx = Width() - x;
			args.Painter.DrawString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), GetForeColor(this->State), Rect(x, 0, cx, (float)_rect.Height), TextAlign, _underline);
		}
		int y = (Height() - fontHeight) / 2;
		args.Painter.FillRectangle(Rect(0, y, fontHeight, fontHeight), Color::White,  fontHeight);
		if (!_checked) {
			args.Painter.FillRectangle(Rect(0 + 2, y + 2, fontHeight - 4, fontHeight - 4), Color::Gray,  fontHeight - 4);
		}
		else {
			args.Painter.FillRectangle(Rect(0 + 2, y + 2, fontHeight - 4, fontHeight - 4), Color(200, 0, 160, 0),  fontHeight - 4);
		}

	}
};