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
		VLayout(Object* parentObject = NULL);
		virtual ~VLayout();

	};
	using VBox = VLayout;
};