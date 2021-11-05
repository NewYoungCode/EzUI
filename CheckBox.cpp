#include "CheckBox.h"

CheckBox::CheckBox()
{
	Cursor = IDC_HAND;
	SetTextAlign(TextAlign::MiddleLeft);

}

CheckBox::~CheckBox()
{
}

void CheckBox::SetCheck(bool checked)
{
	_checked = checked;
	Refresh();
}

bool CheckBox::GetCheck()
{
	return _checked;
}

void CheckBox::OnMouseClick(MouseButton btn, const Point&pt) {
	__super::OnMouseClick(btn, pt);
	SetCheck(!_checked);
}

void CheckBox::OnForePaint(PaintEventArgs & args)
{
	int fontHeight = Height();
	if (!_text.empty()) {
		RectF box;
		args.Painter.MeasureString(_text, GetFontFamily(this->State), GetFontSize(this->State), box);
		fontHeight = (int)box.Height-2;
		int x = fontHeight + Indent;
		int cx = Width() - x;
		args.Painter.DrawString(_text, GetFontFamily(this->State), GetFontSize(this->State), GetForeColor(this->State), RectF(x, 0, cx, (float)_rect.Height), _textAlign, _underline);
	}
	int y = (Height() - fontHeight) / 2;
	args.Painter.FillRectangle(Color(200, 255, 255, 255), { 0,y,fontHeight,fontHeight });
	if (!_checked) {
		args.Painter.FillRectangle(Color(200, 0, 0, 0), { 0 + 2,y + 2,fontHeight - 4,fontHeight - 4 });
	}
	else {
		args.Painter.FillRectangle(Color(200, 0, 160, 0), { 0 + 3,y + 3,fontHeight - 6,fontHeight - 6 }, 2);
	}
}
