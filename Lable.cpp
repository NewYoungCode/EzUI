#include "Lable.h"
Lable::Lable()
{
}

Lable::~Lable()
{

}
void Lable::OnForePaint(PaintEventArgs& args)
{
	__super::OnForePaint(args);
	if (!_text.empty()) {
		args.Painter.MeasureString(_text, GetFontFamily(this->State), GetFontSize(this->State),fontBox);
		args.Painter.DrawString(_text, GetFontFamily(this->State), GetFontSize(this->State), GetForeColor(this->State), RectF(0, 0, (float)_rect.Width, (float)_rect.Height), _textAlign, _underline);
	}
}
Size Lable::GetFontWidth() {

	return Size((int)fontBox.Width,(int)fontBox.Height);
}

void Lable::SetTextAlign(TextAlign textAlign)
{
	_textAlign = textAlign;
}

void Lable::SetText(const EString& text) {
	_text = text;
}

void Lable::SetUnderline(bool enable)
{
	_underline = enable;
}

EString& Lable::GetText()
{
	return _text;
}
