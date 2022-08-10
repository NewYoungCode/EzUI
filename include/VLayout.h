#pragma once
#include "Layout.h"
class UI_EXPORT  VLayout :
	public Layout
{
public:
	virtual ~VLayout();
	void Sort();
public:
	void AddControl(Control*ctl) override;
	void OnSize(const Size&size)override;
	void RefreshLayout()override;
};
using VBox=VLayout;