#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		int_t _pageIndex = 0;
		ThreadTimer timer;
		int_t offset = 0;
		int_t nowOffset = 0;
		void Sort();
	protected:
		virtual void OnLayout()override;
		virtual Control* Add(Control* childCtl)override;
		virtual void SetAttribute(const EString& key, const EString& value)override;
	public:
		TabLayout();
		virtual ~TabLayout();
		virtual void Remove(Control* ctl, bool freeCtl = false)override;
		void SetPageIndex(int_t index);
		void SlideToPage(int_t index);
		void SetPage(Control* ctl);
		Control* GetPage();
		int_t GetPageIndex();
	};
};