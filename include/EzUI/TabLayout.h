#pragma once
#include "Control.h"
#include "Timer.h"
#include "Animation.h"
namespace ezui {
	//滑动方向
	enum class SlideDirection :int16_t {
		Horizontal,  // 横向滑动（比如从左滑到右）
		Vertical     // 纵向滑动（比如从上滑到底）
	};
	class UI_EXPORT TabLayout :
		public Control
	{
	private:
		SlideDirection m_dlideDirection = SlideDirection::Horizontal;
		int m_pageIndex = 0;
		Animation* m_amt;
		std::vector<int> m_initial;
		void Sort();
		void Init();
	protected:
		virtual void OnLayout()override;
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual void OnDpiChanged(const DpiChangedEventArgs& arg)override;
	public:
		TabLayout(Object* ownerObject = NULL);
		virtual ~TabLayout();
		virtual void RemoveChild(Control* ctl, bool freeCtl = false)override;
		virtual Control* AddChild(Control* childCtl)override;
		//设置当前显示页
		void SetPageIndex(int index);
		//动画方式滑动到某一页
		void SlideToPage(int index, SlideDirection dlideDirection = SlideDirection::Horizontal, int durationMs = 200);
		void SetPage(Control* ctrl);
		Control* GetPage();
		//获取当前页索引
		int GetPageIndex();
	};
};