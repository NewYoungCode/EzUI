#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT  Layout :
		public Control
	{
	public:
		Layout();
	/*	void AddControl(Control* ctl) override;
		virtual ControlIterator RemoveControl(Control* ctl)override;
		virtual void OnSize(const Size& size)override;*/
		virtual void ResumeLayout()override;
		virtual ~Layout();
	};
	using Box = Layout;
};