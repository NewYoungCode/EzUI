#pragma once
#include "PagedListView.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT  TileListView :
		public PagedListView
	{
	private:
		VScrollBar _vScrollBar;
		bool _autoHeight = false;//根据内容的高度自动变化
		void Init();
		void Offset(int_t offset);
	protected:
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		TileListView();
		virtual ~TileListView();
		virtual IScrollBar* GetScrollBar()override;
	};
};