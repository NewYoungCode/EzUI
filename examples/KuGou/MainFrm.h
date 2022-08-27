#pragma once
#include "global.h"
class MainFrm :
	public Form
{
private:
	void InitForm();
	VList* localList;
	VList* searchList;
	Edit* searchEdit;
protected:
	virtual void OnKeyDown(WPARAM wparam)override;
	bool OnNotify(Control* sender, const EventArgs& args)override;
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;

};

