#pragma once
#include "Control.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Control
	{
	protected:
		int _index = 0;
	public:
		TabLayout();
		TabLayout(Control* parent);;
		virtual ~TabLayout();
		void SetPageIndex(int index);
		virtual void AddControl(Control* ctl);
		ControlIterator RemoveControl(Control* ctl);
		int GetPageIndex();
		virtual void OnLayout();
		virtual void OnSize(const Size& sz);
	};
};