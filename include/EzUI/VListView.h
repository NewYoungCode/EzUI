#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	class UI_EXPORT VListView :
		public PagedListView
	{
	private:
		VScrollBar _vScrollBar;
		void Init();
		//对控件进行偏移
		void Offset(int_t offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		VListView();
		virtual ~VListView();
		virtual IScrollBar* GetScrollBar() override;
	};
};