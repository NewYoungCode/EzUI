#pragma once
#include "Control.h"
class UI_EXPORT  Layout :
	public Control
{
public:
	Layout();
	virtual ~Layout();
	virtual void Sort();//重新布局
	virtual void RefreshLayout();//刷新布局并重绘
};

