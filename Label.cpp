#include "Label.h"
Label::Label()
{
}

Label::~Label()
{

}
void Label::OnForePaint(PaintEventArgs& args)
{
	__super::OnForePaint(args);
	if (!_text.empty()) {
		args.Painter.MeasureString(_text, GetFontFamily(this->State), GetFontSize(this->State),fontBox);
		args.Painter.DrawString(_text, GetFontFamily(this->State), GetFontSize(this->State), GetForeColor(this->State), RectF(0, 0, (float)_rect.Width, (float)_rect.Height), _textAlign, _underline);
	}
}
Size Label::GetFontWidth() {

	return Size((int)fontBox.Width,(int)fontBox.Height);
}

void Label::SetTextAlign(TextAlign textAlign)
{
	_textAlign = textAlign;
}

void Label::SetText(const EString& text) {
	_text = text;
}

void Label::SetUnderline(bool enable)
{
	_underline = enable;
}

EString& Label::GetText()
{
	return _text;
}
