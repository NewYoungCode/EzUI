#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	class UI_EXPORT  TileListView :
		public PagedListView
	{
	private:
		VScrollBar m_vScrollBar;
		bool m_autoHeight = false;//根据内容的高度自动变化
		void Init();
		void Offset(int offset);
	protected:
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		TileListView(Object* ownerObject = NULL);
		virtual ~TileListView();
		virtual ScrollBar* GetScrollBar()override;
	};
};