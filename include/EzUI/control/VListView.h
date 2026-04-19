#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	//垂直列表视图
	class EZUI_API VListView :
		public PagedListView
	{
	private:
		HAlign m_itemsAlign = HAlign::Center;
		VScrollBar* m_vScrollBar;
		void Init();
		//对控件进行偏移
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs* args)override;
		virtual	void OnScroll(int offsetX, int offsetY)override;
	public:
		VListView(Object* ownerObject = NULL);

		virtual ~VListView();

		//设置子控件的水平对齐方式
		void SetItemsAlign(HAlign itemsAlign);

		// Supported attributes:
		// halign | align: left | right
		// scrollbar: name (not implemented in code)
		// All PagedListView attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)override;

		virtual VScrollBar* GetVScrollBar() override;
	};
};