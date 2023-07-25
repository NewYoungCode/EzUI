#include "Label.h"
namespace EzUI {
	Label::Label() {}
	Label::Label(Control* parent) :Control(parent) {}
	Label::~Label() {}
	void Label::Rending(PaintEventArgs& args) {
		if (AutoWidth && AutoHeight) {
			Size oldSize(Width(), Height());
			std::wstring fontFamily =args.Style.FontFamily .utf16();
			const int fontSize = args.Style.FontSize;
			Font font(fontFamily, fontSize);
			TextLayout textLayout(_wstr, font);
			Size fontBox = textLayout.GetFontBox();
			if (fontBox != oldSize) {
				SetFixedSize({ fontBox.Width,fontBox.Height });
				if (Parent) {
					Parent->Invalidate();
					return;
				}
			}
		}
		__super::Rending(args);
	}
	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (!_wstr.empty()) {
			std::wstring drawText(_wstr);
			std::wstring fontFamily = args.Style.FontFamily.utf16();
			const int fontSize = args.Style.FontSize;
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(args.Style.ForeColor);
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
					int fontHeight;
					textLayout.HitTestPoint({ Width(),0 }, &pos, &isTrailingHit, &fontHeight);//�����ֽ������в���
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
			TextLayout textLayout(viewStr, font, Size(Width(), Height()), (AutoWidth && AutoHeight) ? TextAlign::TopLeft : this->TextAlign);
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
			if (key == "autosize" && value == "true") {
				this->AutoWidth = true;
				this->AutoHeight = true;
				return;
			}
			if (key == "width" && value == "auto") {
				this->AutoWidth = true;
				return;
			}
			if (key == "height" && value == "auto") {
				this->AutoHeight = true;
				return;
			}
		} while (false);
		__super::SetAttribute(key, value);
	}
	void Label::SetText(const EString& text) {
		_wstr = text.utf16();
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