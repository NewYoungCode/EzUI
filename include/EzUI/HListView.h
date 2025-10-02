#pragma once
#include "PagedListView.h"
#include "HScrollBar.h"

namespace ezui {
	class UI_EXPORT  HListView :
		public PagedListView
	{
	private:
		VAlign m_contentAlign = VAlign::Mid;
		HScrollBar m_hScrollBar;
		void Init();
		void Offset(int offset);
	public:
		//布局完成的回调
		std::function<void()> Layout = NULL;
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		HListView(Object* ownerObject = NULL);
		virtual ~HListView();
		void SetContentAlign(VAlign contentAlign);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual ScrollBar* GetScrollBar()override;
	};
};