#pragma once
#include "Control.h"
#include "core/Animation.h"
namespace ezui {
	//滑动方向
	EZUI_SCOPED_ENUM_BEGIN(SlideDirection, int16_t) {
		Horizontal,  // 横向滑动（比如从左滑到右）
		Vertical     // 纵向滑动（比如从上滑到底）
	} EZUI_SCOPED_ENUM_END(SlideDirection)

	//选项卡控件
	class EZUI_API TabControl :
		public Control
	{
	private:
		SlideDirection m_slideDirection;
		int m_pageIndex;
		Animation* m_ani;
		std::vector<int> m_initial;
		int m_slideTargetIndex;
		void Sort();
		void Init();
		void InitPositions();
	protected:
		virtual void OnLayout()EZUI_OVERRIDE;
		virtual void OnDpiChanged(DpiChangedEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnSize(SizeEventArgs* args)EZUI_OVERRIDE;
	public:
		TabControl(Object* ownerObject = NULL);

		virtual ~TabControl();

		/// 移除子控件；deleteChild 为 true 时同时立即删除该控件。
		virtual bool RemoveChild(Control* ctrl, bool deleteChild = false)EZUI_OVERRIDE;

		/// 插入子控件；takeOwnership 为 true 时同时取得对象所有权。
		virtual Control* InsertChildAt(int pos, Control* ctrl, bool takeOwnership = false)EZUI_OVERRIDE;

		// Supported attributes:
		// tabindex | pageindex: number
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)EZUI_OVERRIDE;

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
