#pragma once
#include "PagedListView.h"
#include "HScrollBar.h"

namespace ezui {
	class EZUI_API HListView :
		public PagedListView
	{
	private:
		VAlign m_itemsAlign;
		HScrollBar* m_hScrollBar;
		void Init();
	protected:
		virtual void OnLayout()EZUI_OVERRIDE;
		virtual void OnChildPaint(PaintEventArgs* args)EZUI_OVERRIDE;
		virtual void OnScroll(int offsetX, int offsetY)EZUI_OVERRIDE;
	public:
		HListView(Object* ownerObject = NULL);

		//设置子控件的垂直对齐方式
		void SetItemsAlign(VAlign itemsAlign);

		// Supported attributes:
		// valign | align: top | bottom
		// scrollbar: name (not implemented in code)
		// All PagedListView attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)EZUI_OVERRIDE;

		virtual HScrollBar* GetHScrollBar()EZUI_OVERRIDE;

		virtual ~HListView();
	};
};
