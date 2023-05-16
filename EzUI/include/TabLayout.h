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
		int GetPageIndex();
		virtual void OnLayout();
	};
};