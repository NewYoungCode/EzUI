#pragma once
#include "Control.h"
class UI_EXPORT  Layout :
	public Control
{
public:
	Layout();
	virtual ~Layout();
	virtual void Sort();//���²���
	virtual void RefreshLayout();//ˢ�²��ֲ��ػ�
};

