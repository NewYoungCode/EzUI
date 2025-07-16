#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  HLayout :
		public Control
	{
	public:
		VAlign ContentAlign = VAlign::Mid;
	protected:
		virtual void OnLayout()override;
	public:
		HLayout();
		HLayout(Control*parentObject);
		virtual ~HLayout();
	};
	using HBox = HLayout;
};