#include "Label.h"
namespace EzUI {
	Label::Label() {}
	Label::~Label() {
		if (_textLayout) {
			delete _textLayout;
		}
	}
	void Label::DoPaint(PaintEventArgs& args, bool paintSelf = true) {
		if (IsAutoWidth() || IsAutoHeight()) {
			int fontSize = GetFontSize();
			std::wstring fontFamily = GetFontFamily();
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(GetForeColor());
			if (_textLayout == NULL) {
				_textLayout = new TextLayout(_wstr, font, { Width(),Height() }, this->TextAlign);
			}
			if (_textLayout->GetFontSize() != fontSize || _textLayout->GetFontFamily() != fontFamily) {
				delete _textLayout;
				_textLayout = new TextLayout(_wstr, font, { Width(),Height() }, this->TextAlign);
			}
			if (IsAutoWidth() && _textLayout->Width() != Width()) {
				this->SetFixedWidth(_textLayout->Width());
				delete _textLayout;
				_textLayout = NULL;
				if (Parent) {
					Parent->Invalidate();
				}
				return;
			}
			if (IsAutoHeight() && _textLayout->Height() != Height()) {
				this->SetFixedHeight(_textLayout->Height());
				delete _textLayout;
				_textLayout = NULL;
				if (Parent) {
					Parent->Invalidate();
				}
				return;
			}
		}
		__super::DoPaint(args, paintSelf);
	}
	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (_textLayout) {
			args.Graphics.DrawString(*_textLayout);
		}
		else if (!_wstr.empty()) {
			std::wstring drawText(_wstr);
			std::wstring fontFamily = GetFontFamily();
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
			TextLayout textLayout(viewStr, font, Size(Width(), Height()), (IsAutoWidth() && IsAutoHeight()) ? TextAlign::TopLeft : this->TextAlign);
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
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)VAlign::Top);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)VAlign::Mid);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)VAlign::Bottom);
				VAlign v = VAlign::Mid;
				if (value == "top") {
					v = VAlign::Top;
				}
				else if (value == "bottom") {
					v = VAlign::Bottom;
				}
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign | (int)v);
				break;
			}
			if (key == "halign") {
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)HAlign::Left);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)HAlign::Center);
				this->TextAlign = EzUI::TextAlign((int)this->TextAlign & ~(int)HAlign::Right);
				HAlign h = HAlign::Center;
				if (value == "left") {
					h = HAlign::Left;
				}
				else if (value == "right") {
					h = HAlign::Right;
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
	void Label::ResumeLayout()
	{
		//�Ƚ�������Ҫ����
		this->OnLayout();
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