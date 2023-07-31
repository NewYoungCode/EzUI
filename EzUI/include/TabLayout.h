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
		virtual ~TabLayout();
		void SetPageIndex(int index);
		int GetPageIndex();
		virtual void OnLayout();
	};
};