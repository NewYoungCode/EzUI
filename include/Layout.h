#pragma once
#include "Container.h"

namespace EzUI {

	class UI_EXPORT  Layout :
		public Container
	{
	public:
		Layout();
		virtual ~Layout();
		virtual void Sort();//���²���
		virtual void RefreshLayout();//ˢ�²��ֲ��ػ�
	};
	using Box = Layout;
};