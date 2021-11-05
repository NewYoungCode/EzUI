#pragma once
#include "Container.h"
#include "HScrollBar.h"
class UI_EXPORT  HList :
	public Container
{
private:
	HScrollBar *hScrollBar = NULL;
public:
	
	int _maxRight = 0;
public:
	HList();
	virtual ~HList();
	virtual	void SetMargin(int margin);
	virtual	void AddControl(Control*ctl) override;
	virtual		ControlIterator RemoveControl(Control * ctl)override;
	virtual		void Clear();
	virtual	void OnSize(const Size & size) override;
	void OnChildPaint(Controls & controls, PaintEventArgs & args)override;
};