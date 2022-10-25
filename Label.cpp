#include "Label.h"
namespace EzUI {

	Label::Label()
	{
	}

	Label::~Label()
	{

	}
	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (!_wstr.empty()) {
			//args.Painter.MeasureString(_text, GetFontFamily(this->State), GetFontSize(this->State), fontBox);
			args.Painter.DrawString(_wstr, GetFontFamily(this->State).utf16(), GetFontSize(this->State), GetForeColor(this->State), Rect(0, 0, Width(), Height()), TextAlign, _underline);
		}
	}

	void Label::SetAttribute(const EString& key, const EString& value) {
		do
		{
			if (key == "valign") {
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Top);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Mid);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Bottom);
				Align v = Align::Mid;
				if (value == "top") {
					v = Align::Top;
				}
				else if (value == "bottom") {
					v = Align::Bottom;
				}
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign | (int)v);
				break;
			}
			if (key == "halign") {
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Left);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Center);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)Align::Right);
				Align h = Align::Center;
				if (value == "left") {
					h = Align::Left;
				}
				else if (value == "right") {
					h = Align::Right;
				}
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign | (int)h);
				break;
			}
			if (key == "text") {
				this->SetText(value);
				break;
			}
		} while (false);
		__super::SetAttribute(key, value);
	}
	Size Label::GetFontWidth() {

		return Size((int)fontBox.Width, (int)fontBox.Height);
	}

	void Label::SetText(const EString& text) {
		_wstr = text.utf16();
		//if (AutoWidth || AutoHeight) {
		//	RectF box;
		//	//计算字体的长度
		//	HDC dc = ::GetDC(NULL);
		//	/*Gdiplus::Graphics gp(dc);
		//	std::wstring nickname = _text.utf16();
		//	Gdiplus::FontFamily ff(GetFontFamily(this->State).utf16().c_str());
		//	Gdiplus::Font f(&ff, GetFontSize(this->State));
		//	gp.MeasureString(nickname.c_str(), nickname.size(), &f, PointF(0, 0), &box);*/
		//	int autoW = box.Width + 1;
		//	int autoH = box.Height + 1;
		//	if (AutoWidth) {
		//		SetFixedWidth(autoW);
		//	}
		//	if (AutoHeight) {
		//		SetFixedHeight(autoH);
		//	}
		//	::ReleaseDC(NULL, dc);
		//	Layout* pCtl = dynamic_cast<Layout*>(Parent);
		//	if (pCtl) {
		//		pCtl->ResumeLayout();
		//	}
		//}
	}

	void Label::SetUnderline(bool enable)
	{
		_underline = enable;
	}

	EString Label::GetText()const
	{
		return EString(_wstr);
	}
};