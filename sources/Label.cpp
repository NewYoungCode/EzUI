#include "Label.h"
namespace ezui {
	Label::Label() {}
	Label::Label(Control* parentObject):Control(parentObject)
	{
	}
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
		if (!m_wstr.empty()) {
			int_t maxWidth = Width() - this->TextMargin.GetHSpace();
			int_t maxHeight = Height() - this->TextMargin.GetVSpace();
			std::wstring drawText(m_wstr);
			std::wstring fontFamily = GetFontFamily();
			auto fontSize = GetFontSize();
			if (fontSize == 0)return;
			Font font(fontFamily, fontSize);
			args.Graphics.SetFont(font);
			args.Graphics.SetColor(GetForeColor());
			std::wstring wEllipsisText = Ellipsis.unicode();
			if (!wEllipsisText.empty()) { //水平文本溢出的显示方案
				Size ellipsisTextSize;
				{
					TextLayout textLayout(wEllipsisText, font);
					ellipsisTextSize = textLayout.GetFontBox();
				}
				TextLayout textLayout(m_wstr, font);

				if (textLayout.GetFontBox().Width > maxWidth) {//当文字显示超出的时候 宽度
					int_t pos = 0;
					BOOL isTrailingHit;
					int_t fontHeight;
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
			std::wstring viewStr = !drawText.empty() ? drawText : Ellipsis.unicode();
			TextLayout textLayout(viewStr, font, SizeF(maxWidth, maxHeight), (IsAutoWidth() && IsAutoHeight()) ? TextAlign::TopLeft : this->TextAlign);
			if (this->m_underlineCount != 0) {//下划线
				textLayout.SetUnderline(m_underlinePos, m_underlineCount);
			}
			args.Graphics.DrawTextLayout(textLayout, { (float)this->TextMargin.Left,(float)this->TextMargin.Top });
		}
	}

	void Label::SetAttribute(const UIString& key, const UIString& value) {
		do
		{
			if (key == "valign") {
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)VAlign::Top);
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)VAlign::Mid);
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)VAlign::Bottom);
				VAlign v = VAlign::Mid;
				if (value == "top") {
					v = VAlign::Top;
				}
				else if (value == "bottom") {
					v = VAlign::Bottom;
				}
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign | (int_t)v);
				break;
			}
			if (key == "ellipsis") {
				this->Ellipsis = value;
				break;
			}
			if (key == "halign") {
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)HAlign::Left);
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)HAlign::Center);
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign & ~(int_t)HAlign::Right);
				HAlign h = HAlign::Center;
				if (value == "left") {
					h = HAlign::Left;
				}
				else if (value == "right") {
					h = HAlign::Right;
				}
				this->TextAlign = ezui::TextAlign((int_t)this->TextAlign | (int_t)h);
				break;
			}
			if (key == "text") {
				this->SetText(value);
				break;
			}
			if (key == "underline") {
				size_t pos = value.find(",");
				this->m_underlinePos = std::atoi(value.substr(0, pos + 1).c_str());
				this->m_underlineCount = std::atoi(value.substr(pos + 1, pos).c_str());
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

			int_t maxWidth = IsAutoWidth() ? __MAXFLOAT : Width() - this->TextMargin.GetHSpace();
			int_t maxHeight = IsAutoHeight() ? __MAXFLOAT : Height() - this->TextMargin.GetVSpace();

			TextLayout text(this->m_wstr, font, SizeF(maxWidth, maxHeight));
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
	void Label::SetText(const UIString& text) {
		m_wstr = text.unicode();
		this->TryPendLayout();
	}
	void Label::SetUnderline(size_t pos, size_t count)
	{
		this->m_underlinePos = pos;
		this->m_underlineCount = count;
	}
	UIString Label::GetText()const
	{
		return UIString(m_wstr);
	}
};