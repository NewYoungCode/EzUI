#pragma once
#include "Control.h"
class UI_EXPORT Container :
	public Control
{
public:
	std::map<Control*, int> _controlsLocationX;
	std::map<Control*, int> _controlsLocationY;
public:
	int Margin = 0;//�����ˮƽ�б��Ǿ��� �ұ߾� ,��ֱ�����������±߾�
	Container();
	virtual ~Container();
	virtual void RefreshLayout();
};
