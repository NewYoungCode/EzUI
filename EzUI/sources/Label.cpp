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
			std::wstring drawText(_wstr);
			std::wstring fontF = GetFontFamily().utf16();
			UI_Int fontSize = GetFontSize();
			Color fontColor = GetForeColor();
			if (!EllipsisText.empty()) { //水平文本溢出的显示方案
				Size ellipsisTextSize;
				TextFormat textFormat(fontF, fontSize, TextAlign::TopLeft);
				{
					TextLayout textLayout(EllipsisText.utf16(), { 16777216, Height() }, &textFormat);
					ellipsisTextSize = textLayout.GetFontSize();
				}
				TextLayout textLayout(_wstr, { 16777216, Height() }, &textFormat);
				if (textLayout.GetFontSize().Width > Width()) {//当文字显示超出的时候 宽度
					int pos = 0;
					BOOL isTrailingHit;
					textLayout.HitTestPoint({ Width(),0 }, pos, isTrailingHit);//对文字进行命中测试
					drawText.erase(pos);
					while (drawText.size() > 0)
					{
						//从最后往前删除文字 直到可以显示正常为止
						drawText.erase(drawText.size() - 1, 1);
						TextLayout textLayout(drawText, { 16777216, Height() }, &textFormat);
						if (textLayout.GetFontSize().Width + ellipsisTextSize.Width < Width()) {
							drawText.append(EllipsisText.utf16());
							break;
						}
					}
				}
			}
			EzUI::DrawString(args.Painter,!drawText.empty() ? drawText : EllipsisText.utf16(), fontF, fontSize, fontColor, Rect(0, 0, Width(), Height()), TextAlign, _underline);
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