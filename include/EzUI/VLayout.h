#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  VLayout :
		public Control
	{
	public:
		HAlign ContentAlign = HAlign::Center;
	protected:
		virtual void OnLayout() override;
	public:
		VLayout();
		VLayout(Control* parentObject);
		virtual ~VLayout();

	};
	using VBox = VLayout;
};