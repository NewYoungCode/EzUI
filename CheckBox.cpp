#include "CheckBox.h"

namespace EzUI {

	CheckBox::CheckBox()
	{
		//Cursor = IDC_HAND;
		TextAlign=TextAlign::MiddleLeft;

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

	void CheckBox::OnForePaint(PaintEventArgs& args)
	{
		int fontHeight = Height();
		if (!_text.empty()) {
			/*RectF box;
			args.Painter.MeasureString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), box);
			fontHeight = (int)box.Height - 2;
			int x = fontHeight + Indent;
			int cx = Width() - x;
			args.Painter.DrawString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), GetForeColor(this->State), Rect(x, 0, cx, (float)_rect.Height), TextAlign, _underline);*/
		}
		int y = (Height() - fontHeight) / 2;
		args.Painter.FillRectangle({ 0,y,fontHeight,fontHeight },Color(200, 255, 255, 255));
		if (!_checked) {
			args.Painter.FillRectangle({ 0 + 2,y + 2,fontHeight - 4,fontHeight - 4 },Color(200, 0, 0, 0));
		}
		else {
			args.Painter.FillRectangle({ 0 + 3,y + 3,fontHeight - 6,fontHeight - 6 },Color(200, 0, 160, 0), 2);
		}
	}
};