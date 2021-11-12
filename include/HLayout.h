#pragma once
#include "Layout.h"
class UI_EXPORT  HLayout :
	public Layout
{
public:
	void Sort();
public:
	void AddControl(Control*ctl) override;
	void OnSize(const Size&size)override;
	void RefreshLayout()override;
};
