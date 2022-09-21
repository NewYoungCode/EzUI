#pragma once
#include "Layout.h"

namespace EzUI {

	class UI_EXPORT  VLayout :
		public Layout
	{
	public:
		virtual ~VLayout();
	public:
		void AddControl(Control* ctl) override;
		ControlIterator RemoveControl(Control* ctl)override;
		void OnSize(const Size& size)override;
		void ResumeLayout()override;
	};
	using VBox = VLayout;
};