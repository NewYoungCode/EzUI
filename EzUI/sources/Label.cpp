#include "Label.h"
namespace EzUI {
	Label::Label() {}
	Label::~Label() {}
	void Label::OnDpiChange(const DpiChangeEventArgs& args) {
		if (args.Scale != this->GetScale()) {
			this->TextMargin.Scale(args.Scale / this->GetScale());
		}
		__super::OnDpiChange(args);
	}

	void Label::OnForePaint(PaintEventArgs& args)
	{
		__super::OnForePaint(args);
		if (!_wstr.empty()) {
			int maxWidth = Width() - this->TextMargin.GetHSpace();
			int maxHeight = Height() - this->TextMargin.GetVSpace();
			std::wstring drawText(_wstr);
			std::wstring fontFamily = GetFontFamily();
			auto fontSize = GetFontSize();
			if (fontSize == 0)return;
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(GetForeColor());
			std::wstring wEllipsisText = Ellipsis.utf16();
			if (!wEllipsisText.empty()) { //水平文本溢出的显示方案
				Size ellipsisTextSize;
				{
					TextLayout textLayout(wEllipsisText, font);
					ellipsisTextSize = textLayout.GetFontBox();
				}
				TextLayout textLayout(_wstr, font);

				if (textLayout.GetFontBox().Width > maxWidth) {//当文字显示超出的时候 宽度
					int pos = 0;
					BOOL isTrailingHit;
					int fontHeight;
					textLayout.HitTestPoint({ maxWidth,0 }, &pos, &isTrailingHit, &fontHeight);//对文字进行命中测试
					drawText.erase(pos);
					while (drawText.size() > 0)
					{
						//从最后往前删除文字 直到可以显示正常为止
						drawText.erase(drawText.size() - 1, 1);
						TextLayout textLayout(drawText, font);
						if (textLayout.GetFontBox().Width + ellipsisTextSize.Width < maxWidth) {
							drawText.append(wEllipsisText);
							break;
						}
					}
				}
			}
			std::wstring viewStr = !drawText.empty() ? drawText : Ellipsis.utf16();
			TextLayout textLayout(viewStr, font, Size(maxWidth, maxHeight), (IsAutoWidth() && IsAutoHeight()) ? TextAlign::TopLeft : this->TextAlign);
			if (this->_underlineCount != 0) {//下划线
				textLayout.SetUnderline(_underlinePos, _underlineCount);
			}
			args.Graphics.DrawTextLayout(textLayout, { (int)this->TextMargin.Left,(int)this->TextMargin.Top });
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
			if (key == "ellipsis") {
				this->Ellipsis = value;
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
			if (key == "underline") {
				size_t pos = value.find(",");
				this->_underlinePos = std::atoi(value.substr(0, pos + 1).c_str());
				this->_underlineCount = std::atoi(value.substr(pos + 1, pos).c_str());
				break;
			}

		} while (false);
		__super::SetAttribute(key, value);
	}
	void Label::RefreshLayout()
	{
		//比较特殊需要屏蔽
		this->OnLayout();
	}
	void Label::OnLayout() {
		__super::OnLayout();
		if (IsAutoWidth() || IsAutoHeight()) {

			auto fontSize = GetFontSize();
			if (fontSize == 0)return;
			Font font(GetFontFamily(), fontSize);

			int maxWidth = IsAutoWidth() ? __MAXFLOAT : Width() - this->TextMargin.GetHSpace();
			int maxHeight = IsAutoHeight() ? __MAXFLOAT : Height() - this->TextMargin.GetVSpace();

			TextLayout text(this->_wstr, font, Size(maxWidth, maxHeight));
			Size box = text.GetFontBox();
			this->SetContentSize(box);
			if (IsAutoWidth()) {
				this->SetFixedWidth(box.Width + this->TextMargin.GetHSpace());
			}
			if (IsAutoHeight()) {
				this->SetFixedHeight(box.Height + this->TextMargin.GetVSpace());
			}
		}
	}
	void Label::SetText(const EString& text) {
		_wstr = text.utf16();
		this->TryPendLayout();
	}
	void Label::SetUnderline(size_t pos, size_t count)
	{
		this->_underlinePos = pos;
		this->_underlineCount = count;
	}
	EString Label::GetText()const
	{
		return EString(_wstr);
	}
};