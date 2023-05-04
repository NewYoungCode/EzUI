#include "Label.h"
namespace EzUI {
	Label::Label() {}
	Label::Label(Control* parent) :Control(parent) {}
	Label::~Label() {}
	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (!_wstr.empty()) {
			std::wstring drawText(_wstr);

			std::wstring fontFamily = GetFontFamily().utf16();
			int fontSize = GetFontSize();
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(GetForeColor());

			std::wstring wEllipsisText = EllipsisText.utf16();

			if (!wEllipsisText.empty()) { //ˮƽ�ı��������ʾ����
				Size ellipsisTextSize;
				{
					TextLayout textLayout(wEllipsisText, font);
					ellipsisTextSize = textLayout.GetFontBox();
				}
				TextLayout textLayout(_wstr, font);
				if (textLayout.GetFontBox().Width > Width()) {//��������ʾ������ʱ�� ���
					int pos = 0;
					BOOL isTrailingHit;
					textLayout.HitTestPoint({ Width(),0 }, pos, isTrailingHit);//�����ֽ������в���
					drawText.erase(pos);
					while (drawText.size() > 0)
					{
						//�������ǰɾ������ ֱ��������ʾ����Ϊֹ
						drawText.erase(drawText.size() - 1, 1);
						TextLayout textLayout(drawText, font);
						if (textLayout.GetFontBox().Width + ellipsisTextSize.Width < Width()) {
							drawText.append(wEllipsisText);
							break;
						}
					}
				}
			}
			std::wstring viewStr = !drawText.empty() ? drawText : EllipsisText.utf16();
			TextLayout textLayout(viewStr, font, this->TextAlign, Size(Width(), Height()));
			if (this->_underline) {//�»���
				textLayout.SetUnderline(0, viewStr.size());
			}
			args.Graphics.DrawString(textLayout);
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
		//	//��������ĳ���
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