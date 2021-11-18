#pragma once
#include "Control.h"
class UI_EXPORT  Lable :
	public Control
{
protected:
	TextAlign _textAlign = TextAlign::MiddleCenter;
	EString _text;
	bool _underline = false;
	RectF fontBox;
protected:
	virtual void OnForePaint( PaintEventArgs&args) override;
public:
	bool AutoWidth = false;
	Size GetFontWidth();
	Lable();
	virtual ~Lable();
	void SetTextAlign(TextAlign textAlign);
	void SetText(const EString&text);
	void SetUnderline(bool enable = true);
	EString& GetText();
};