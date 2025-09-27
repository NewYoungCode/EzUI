#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	class UI_EXPORT VListView :
		public PagedListView
	{
	private:
		HAlign m_contentAlign = HAlign::Center;
		VScrollBar m_vScrollBar;
		void Init();
		//对控件进行偏移
		void Offset(int offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		VListView(Object* ownerObject = NULL);
		virtual ~VListView();
		void SetContentAlign(HAlign contentAlign);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual ScrollBar* GetScrollBar() override;
	};
};