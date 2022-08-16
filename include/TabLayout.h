#pragma once
#include "Container.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Container
	{
	protected:
		int _index = 0;
	public:
		TabLayout();
		virtual ~TabLayout();
		void SetPageIndex(int index);
		virtual void AddControl(Control* ctl);
		int PageIndex();
		virtual void RefreshLayout();
		virtual void OnSize(const Size& sz);
	};
};