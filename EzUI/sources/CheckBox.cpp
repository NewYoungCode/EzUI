#include "CheckBox.h"

namespace EzUI {

	CheckBox::CheckBox()
	{
		//Cursor = IDC_HAND;
		TextAlign = TextAlign::MiddleLeft;
	}
	CheckBox::CheckBox(Control* parent) :Label(parent)
	{
		//Cursor = IDC_HAND;
		TextAlign = TextAlign::MiddleLeft;
	}
	CheckBox::~CheckBox()
	{
	}
	void CheckBox::SetCheck(bool checked)
	{
		_checked = checked;
		Invalidate();
	}

	bool CheckBox::GetCheck()
	{
		return _checked;
	}

	void CheckBox::OnMouseClick(MouseButton btn, const Point& pt) {
		__super::OnMouseClick(btn, pt);
		SetCheck(!_checked);
	}

	void CheckBox::OnForePaint(PaintEventArgs& e)
	{
		int fontHeight = Height();
		if (!_wstr.empty()) {
			/*RectF box;
			args.Painter.MeasureString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), box);
			fontHeight = (int)box.Height - 2;
			int x = fontHeight + Indent;
			int cx = Width() - x;
			args.Painter.DrawString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), GetForeColor(this->State), Rect(x, 0, cx, (float)_rect.Height), TextAlign, _underline);*/
		}
		int y = (Height() - fontHeight) / 2;

		e.Graphics.SetColor(Color(200, 255, 255, 255));
		e.Graphics.FillRectangle({ 0,y,fontHeight,fontHeight });
		if (!_checked) {
			e.Graphics.SetColor(Color(200, 0, 0, 0));
			e.Graphics.FillRectangle({ 0 + 2,y + 2,fontHeight - 4,fontHeight - 4 });
		}
		else {
			e.Graphics.SetColor(Color(200, 0, 160, 0));
			e.Graphics.FillRectangle({ 0 + 3,y + 3,fontHeight - 6,fontHeight - 6 }, 2);
		}
	}
};