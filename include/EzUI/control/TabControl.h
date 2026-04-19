#pragma once
#include "Control.h"
#include "core/Animation.h"
namespace ezui {
	//滑动方向
	enum class SlideDirection :int16_t {
		Horizontal,  // 横向滑动（比如从左滑到右）
		Vertical     // 纵向滑动（比如从上滑到底）
	};

	//选项卡控件
	class EZUI_API TabControl :
		public Control
	{
	private:
		SlideDirection m_slideDirection = SlideDirection::Horizontal;
		int m_pageIndex = -1;
		Animation* m_ani;
		std::vector<int> m_initial;
		int m_slideTargetIndex = -1;
		void Sort();
		void Init();
		void InitPositions();
	protected:
		virtual void OnLayout()override;
		virtual void OnDpiChanged(DpiChangedEventArgs* arg)override;
		virtual void OnSize(SizeEventArgs* args)override;
	public:
		TabControl(Object* ownerObject = NULL);

		virtual ~TabControl();

		virtual bool RemoveChild(Control* ctrl, bool freeCtrl = false)override;

		virtual Control* InsertChildAt(int pos, Control* ctrl, bool autoDelete = false)override;

		// Supported attributes:
		// tabindex | pageindex: number
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)override;

		//获取当前页索引
		int GetPageIndex();

		//设置当前显示页
		void SetPageIndex(int index);

		//动画方式滑动到某一页
		void SlideToPage(int index, SlideDirection slideDirection = SlideDirection::Horizontal, int durationMs = 300);

		//设置当前显示页
		void SetPage(Control* ctrl);

		//获取当前页 pageIndex为-1则获取当前显示页
		Control* GetPage(int pageIndex = -1);
	};
};
