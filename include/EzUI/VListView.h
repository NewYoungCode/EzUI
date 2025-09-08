#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	class UI_EXPORT VListView :
		public PagedListView
	{
	private:
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
		virtual ScrollBar* GetScrollBar() override;
	};
};