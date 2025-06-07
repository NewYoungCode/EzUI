#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {

	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		int _pageIndex = 0;
		ThreadTimer timer;
		int offset = 0;
		int nowOffset = 0;
		void Sort();
	protected:
		virtual void OnLayout()override;
		virtual Control* Add(Control* childCtl)override;
		virtual void SetAttribute(const EString& key, const EString& value)override;
	public:
		TabLayout();
		virtual ~TabLayout();
		virtual void Remove(Control* ctl, bool freeCtl = false)override;
		void SetPageIndex(int index);
		void SlideToPage(int index);
		void SetPage(Control* ctl);
		Control* GetPage();
		int GetPageIndex();
	};
};