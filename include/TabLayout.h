#pragma once
#include "Control.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Control
	{
		UI_GetClassName()
	protected:
		int _index = 0;
	public:
		TabLayout();
		virtual ~TabLayout();
		void SetPageIndex(int index);
		virtual void AddControl(Control* ctl);
		ControlIterator RemoveControl(Control* ctl);
		int GetPageIndex();
		virtual void OnLayout();
		virtual void OnSize(const Size& sz);
	};
};