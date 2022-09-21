#pragma once
#include "Layout.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Layout
	{
	protected:
		int _index = 0;
	public:
		TabLayout();
		virtual ~TabLayout();
		void SetPageIndex(int index);
		virtual void AddControl(Control* ctl);
		ControlIterator RemoveControl(Control* ctl);
		int PageIndex();
		virtual void ResumeLayout();
		virtual void OnSize(const Size& sz);
	};
};