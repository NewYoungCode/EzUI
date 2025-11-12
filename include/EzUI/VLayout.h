#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  VLayout :
		public Control
	{
	private:
		HAlign m_contentAlign = HAlign::None;
	protected:
		void DistributeAutoHeights(std::vector<Control*> const& autoSizeCtrls, int availableHeight);
		virtual void OnLayout() override;
	public:
		VLayout(Object* ownerObject = NULL);
		// 添加一个弹簧控件:
		// - 传入 fixedHeight > 0,则作为固定高度的空白,位不可伸缩
		// - 传入 fixedHeight == 0,则作为可拉伸的弹簧,占据剩余空间
		void AddSpacer(int fixedHeight = 0);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		//设置子控件的水平对齐方式
		void SetContentAlign(HAlign contentAlign);
		virtual ~VLayout();
	};
	using VBox = VLayout;
};