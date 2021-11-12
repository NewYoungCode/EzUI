#pragma once
#include "Container.h"
#include "VScrollBar.h"
class UI_EXPORT VList :
	public Container
{
private:
	VScrollBar *verticalScrollBar = NULL;
public:
	int _maxBottom = 0;
public:
	VList();
	virtual ~VList();
	virtual	void SetMargin(int margin);
	void RefreshLayout()override;
	virtual	void AddControl(Control*ctl) override;
	virtual		ControlIterator RemoveControl(Control * ctl)override;
	virtual		void Clear();
	virtual	void OnSize(const Size & size) override;
	void OnChildPaint(Controls & controls, PaintEventArgs & args)override;
};
