#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  HLayout :
		public Control
	{
	public:
		VAlign ContentAlign = VAlign::Mid;
	public:
		HLayout();
		virtual ~HLayout();
		void OnLayout()override;
	};
	using HBox = HLayout;
};