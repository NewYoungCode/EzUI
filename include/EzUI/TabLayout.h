#pragma once
#include "Control.h"
#include "Timer.h"

namespace ezui {

	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		int_t _pageIndex = 0;
		Timer timer;
		int_t offset = 0;
		int_t nowOffset = 0;
		std::vector<int_t> initialX;
		float stepAcc = 0;
		float stepPerFrame = 0;
		void Sort();
	protected:
		virtual void OnLayout()override;
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
	public:
		TabLayout();
		virtual ~TabLayout();
		virtual void Remove(Control* ctl, bool freeCtl = false)override;
		virtual Control* Add(Control* childCtl)override;
		//设置当前显示页
		void SetPageIndex(int_t index);
		//动画方式滑动到某一页
		void SlideToPage(int_t index);
		void SetPage(Control* ctl);
		Control* GetPage();
		int_t GetPageIndex();
	};
};