#pragma once
#include "Control.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		int _index = 0;
	protected:
		virtual void OnLayout()override;
	public:
		TabLayout();
		virtual ~TabLayout();
		virtual void Remove(Control* ctl, bool freeCtl = false)override;
		void SetPageIndex(int index);
		void SetPage(Control* ctl);
		Control* GetPage();
		int GetPageIndex();
	};
};