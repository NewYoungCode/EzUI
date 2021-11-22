#pragma once
#include "Label.h"
class UI_EXPORT CheckBox :
	public Label
{
protected:
	bool _checked = false;
public:
	CheckBox();
	virtual	~CheckBox();
	int Indent = 2;//Ëõ½ø
	void SetCheck(bool checked = true);
	bool GetCheck();
	void OnMouseClick(MouseButton btn, const Point&pt)override;
	virtual void OnForePaint(PaintEventArgs &args) override;
};

