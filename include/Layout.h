#pragma once
#include "Container.h"
class UI_EXPORT  Layout :
	public Container
{
public:
	Layout();
	virtual ~Layout();
	virtual void Sort();//���²���
	virtual void RefreshLayout();//ˢ�²��ֲ��ػ�
};

