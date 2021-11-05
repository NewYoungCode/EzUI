#pragma once
#include "Lable.h"
class UI_EXPORT  RadioButton :
	public Lable
{
protected:
	bool _checked = false;
public:
	int Indent = 2;//Ëõ½ø
	RadioButton();
	virtual	~RadioButton();
	void SetCheck(bool checked = true);
	bool GetCheck();
	virtual void OnMouseClick(MouseButton btn, const Point&pt)override;
	virtual void OnForePaint(PaintEventArgs &args) override;
};

