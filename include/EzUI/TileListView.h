#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	class UI_EXPORT TileListView :
		public PagedListView
	{
	private:
		VScrollBar* m_vScrollBar;
		void Init();
		void Offset(int offset);
	protected:
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		TileListView(Object* ownerObject = NULL);
		virtual ~TileListView();
		virtual VScrollBar* GetVScrollBar()override;
	};
};