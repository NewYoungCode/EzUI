#pragma once
#include "Container.h"
#include "VScrollBar.h"
class UI_EXPORT  TileLayout :
	public Container
{
private:
	int _lineCount = 0;
	int _MaxBottom = 0;
	VScrollBar *verticalScrollBar = NULL;
	int Margin = 0;
public:
	int MarginTop = 35;
	int MarginRight = 36;
	void Sort();
	virtual void Clear(bool freeList=false)override;
	TileLayout();
	void AddControl(Control*ctl) override;
	void OnSize(const Size&sz) override;
	void OnChildPaint(Controls & controls, PaintEventArgs & args)override;
	void RefreshLayout()override;
};
