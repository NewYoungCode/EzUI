#pragma once
#include "Control.h"
#include "Timer.h"

namespace ezui {
	//滑动方向
	enum class SlideDirection {
		Horizontal,  // 横向滑动（比如从左滑到右）
		Vertical     // 纵向滑动（比如从上滑到底）
	};
	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		int_t m_pageIndex = 0;
		Timer m_timer;
		int_t m_offset = 0;
		int_t m_nowOffset = 0;
		std::vector<int_t> m_initial;
		SlideDirection m_dlideDirection;
		float m_stepAcc = 0;
		float m_stepPerFrame = 0;
		void Sort();
		void Init();
	protected:
		virtual void OnLayout()override;
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
	public:
		TabLayout(Object* parentObject = NULL);
		virtual ~TabLayout();
		virtual void Remove(Control* ctl, bool freeCtl = false)override;
		virtual Control* Add(Control* childCtl)override;
		//设置当前显示页
		void SetPageIndex(int_t index);
		//动画方式滑动到某一页
		void SlideToPage(int_t index, SlideDirection dlideDirection = SlideDirection::Horizontal, int_t durationMs = 150, int_t fps = 90);
		void SetPage(Control* ctl);
		Control* GetPage();
		int_t GetPageIndex();
	};
};