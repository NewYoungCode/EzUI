#pragma once
#include "Control.h"

namespace ezui {
	//垂直布局控件
	class EZUI_API VLayout :
		public Control
	{
	private:
		Align m_itemsAlign = Align::TopCenter;// 水平方向居中，垂直方向从上到下排列
	protected:
		void DistributeAutoHeights(std::vector<Control*> const& autoSizeCtrls, int availableHeight);
		virtual void OnLayout() override;
	public:
		VLayout(Object* ownerObject = NULL);
		// 添加一个弹簧控件:
		// - 传入 fixedHeight > 0,则作为固定高度的空白,位不可伸缩
		// - 传入 fixedHeight == 0,则作为可拉伸的弹簧,占据剩余空间
		void AddSpacer(int fixedHeight = 0);
		// Supported attributes:
		// valign: top | mid | bottom
		// halign: left | center | right
		// align: can combine vertical + horizontal, e.g. "mid center"
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		//设置子控件的对齐方式
		void SetItemsAlign(Align itemsAlign);
		virtual ~VLayout();
	};
	using VBox = VLayout;
};
