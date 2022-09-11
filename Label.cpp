#include "Label.h"
#include "Layout.h"
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
		if (!_text.empty()) {
			//args.Painter.MeasureString(_text, GetFontFamily(this->State), GetFontSize(this->State), fontBox);
			args.Painter.DrawString(_text, GetFontFamily(this->State), GetFontSize(this->State), GetForeColor(this->State), Rect(0, 0, (float)_rect.Width, (float)_rect.Height), TextAlign, _underline);
		}
	}
	Size Label::GetFontWidth() {

		return Size((int)fontBox.Width, (int)fontBox.Height);
	}

	void Label::SetText(const EString& text) {
		_text = text;
		if (AutoWidth || AutoHeight) {
			RectF box;
			//计算字体的长度
			HDC dc = ::GetDC(NULL);
			Gdiplus::Graphics gp(dc);
			std::wstring nickname = _text.utf16();
			Gdiplus::FontFamily ff(GetFontFamily(this->State).utf16().c_str());
			Gdiplus::Font f(&ff, GetFontSize(this->State));
			gp.MeasureString(nickname.c_str(), nickname.size(), &f, PointF(0, 0), &box);
			int autoW = box.Width + 1;
			int autoH = box.Height + 1;
			if (AutoWidth) {
				SetFixedWidth(autoW);
			}
			if (AutoHeight) {
				SetFixedHeight(autoH);
			}
			::ReleaseDC(NULL, dc);
			Layout* pCtl = dynamic_cast<Layout*>(Parent);
			if (pCtl) {
				pCtl->ResumeLayout();
			}
		}
	}

	void Label::SetUnderline(bool enable)
	{
		_underline = enable;
	}

	EString& Label::GetText()
	{
		return _text;
	}
};