#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"

namespace ezui {
	//瓦片列表视图控件
	class EZUI_API TileListView :
		public PagedListView
	{
	private:
		VScrollBar* m_vScrollBar;
		void Init();
	protected:
		virtual void OnChildPaint(PaintEventArgs* args)override;
		virtual void OnLayout()override;
		virtual void OnScroll(int offsetX, int offsetY)override;
	public:
		TileListView(Object* ownerObject = NULL);
		virtual ~TileListView();
		virtual VScrollBar* GetVScrollBar()override;
	};
};