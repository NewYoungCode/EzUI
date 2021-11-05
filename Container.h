#pragma once
#include "Control.h"
class UI_EXPORT Container :
	public Control
{
public:
	std::map<Control*, int> _controlsLocationX;
	std::map<Control*, int> _controlsLocationY;
public:
	int Margin = 0;//如果是水平列表那就是 右边距 ,垂直滚动条就是下边距
	Container();
	virtual ~Container();
	virtual void RefreshLayout();
};
