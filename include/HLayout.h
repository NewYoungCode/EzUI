#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  HLayout :
		public Control
	{
		UI_GetClassName()
	public:
		void OnSize(const Size& size)override;
		void OnLayout()override;
	};
	using HBox = HLayout;
};