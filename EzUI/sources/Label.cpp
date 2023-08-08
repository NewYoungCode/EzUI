#include "Label.h"
namespace EzUI {
	Label::Label() {}
	Label::~Label() {}
	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (!_wstr.empty()) {
			std::wstring drawText(_wstr);
			std::wstring fontFamily = GetFontFamily();
			int fontSize = GetFontSize();
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(GetForeColor());
			std::wstring wEllipsisText = EllipsisText.utf16();
			if (!wEllipsisText.empty()) { //水平文本溢出的显示方案
				Size ellipsisTextSize;
				{
					TextLayout textLayout(wEllipsisText, font);
					ellipsisTextSize = textLayout.GetFontBox();
				}
				TextLayout textLayout(_wstr, font);
				if (textLayout.GetFontBox().Width > Width()) {//当文字显示超出的时候 宽度
					int pos = 0;
					BOOL isTrailingHit;
					int fontHeight;
					textLayout.HitTestPoint({ Width(),0 }, &pos, &isTrailingHit, &fontHeight);//对文字进行命中测试
					drawText.erase(pos);
					while (drawText.size() > 0)
					{
						//从最后往前删除文字 直到可以显示正常为止
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
			if (this->_underline) {//下划线
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
		//比较特殊需要屏蔽
		this->OnLayout();
	}
	void Label::OnLayout() {
		__super::OnLayout();
		if (IsAutoWidth() || IsAutoHeight()) {
			Font font(GetFontFamily(), GetFontSize());
			TextLayout text(this->_wstr, font);
			Size box = text.GetFontBox();
			this->SetContentSize(box);
			if (IsAutoWidth()) {
				this->SetFixedWidth(box.Width);
			}
			if (IsAutoHeight()) {
				this->SetFixedHeight(box.Height);
			}
		}
	}
	void Label::SetText(const EString& text) {
		_wstr = text.utf16();
		this->TryPendLayout();
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