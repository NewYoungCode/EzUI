#pragma once
#include "stdafx.h"
#include "UIWindow.h"
class MainFrm :
	public Window
{
public:
	MainFrm(int width, int height);
	virtual void OnLoad()override;
	virtual bool OnNotify(Control* sender, EventArgs& arg) override;
	virtual void OnClose(bool& cal)override;
private:
	HLayout mainLayout;
	VList left;
	UIWindow* ui=NULL;
	VList right;
};
