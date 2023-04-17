#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  HLayout :
		public Control
	{
	public:
		HLayout();
		HLayout(Control*parent);
		virtual ~HLayout();
		void OnSize(const Size& size)override;
		void OnLayout()override;
	};
	using HBox = HLayout;
};