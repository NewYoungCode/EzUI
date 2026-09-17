#pragma once
#include "Control.h"

namespace ezui {
	//水平布局控件
	class EZUI_API HLayout :
		public Control
	{
	private:
		Align m_itemsAlign;// 垂直方向居中，水平方向从左到右排列
	protected:
		void DistributeAutoWidths(std::vector<Control*> const& autoSizeCtrls, int availableWidth);
		virtual void OnLayout()EZUI_OVERRIDE;
	public:
		HLayout(Object* ownerObject = NULL);

		// 添加一个弹簧控件:
		// - 传入 fixedWidth > 0,则作为固定宽度,不可拉伸
		// - 传入 fixedWidth == 0,则作为可拉伸的弹簧,占据剩余空间
		void AddSpacer(int fixedWidth = 0);

		// Supported attributes:
		// valign: top | mid | bottom
		// halign: left | center | right
		// align: can combine vertical + horizontal, e.g. "mid center"
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)EZUI_OVERRIDE;

		//设置子控件的对齐方式
		void SetItemsAlign(Align itemsAlign);

		virtual ~HLayout();
	};
	typedef HLayout HBox;
};
