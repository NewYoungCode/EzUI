#pragma once
#include "PagedListView.h"
#include "HScrollBar.h"

namespace ezui {
	class UI_EXPORT  HListView :
		public PagedListView
	{
	private:
		HScrollBar _hScrollBar;
		void Init();
		void Offset(int_t offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		HListView();
		virtual ~HListView();
		virtual IScrollBar* GetScrollBar()override;
	};
};