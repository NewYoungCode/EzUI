#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  HLayout :
		public Control
	{
	public:
		VAlign ContentAlign = VAlign::Mid;
	protected:
		virtual void OnLayout()override;
	public:
		HLayout(Object* ownerObject = NULL);
		// 添加一个弹簧控件:
		// - 传入 fixedWidth > 0,则作为固定高度的空白,位不可伸缩
		// - 传入 fixedWidth == 0,则作为可拉伸的弹簧,占据剩余空间
		void AddSpacer(int fixedWidth = 0);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual ~HLayout();
	};
	using HBox = HLayout;
};