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
	private:
		virtual void SetAutoWidth(bool flag)EZUI_OVERRIDE;
	protected:
		virtual void OnChildPaint(PaintEventArgs* args)EZUI_OVERRIDE;
		virtual void OnLayout()EZUI_OVERRIDE;
		virtual void OnScroll(int offsetX, int offsetY)EZUI_OVERRIDE;
	public:
		TileListView(Object* ownerObject = NULL);
		virtual ~TileListView();
		virtual VScrollBar* GetVScrollBar()EZUI_OVERRIDE;
	};
};