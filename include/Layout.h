#pragma once
#include "Container.h"
class UI_EXPORT  Layout :
	public Container
{
public:
	Layout();
	virtual ~Layout();
	virtual void Sort();//重新布局
	virtual void RefreshLayout();//刷新布局并重绘
};

