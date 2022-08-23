#pragma once
#include "Container.h"
#include "VScrollBar.h"
namespace EzUI {

	class UI_EXPORT  TileLayout :
		public Container
	{
	private:
		int _lineCount = 0;
		int _MaxBottom = 0;
		VScrollBar* vScrollBar = NULL;
		int Margin = 0;
	public:
		TileLayout();
		virtual ~TileLayout();
		int MarginTop = 35;
		int MarginRight = 36;
		void Sort();
		virtual void Clear(bool freeList = false)override;
		virtual void OnSize(const Size& sz) override;
		virtual void OnChildPaint(Controls& controls, PaintEventArgs& args)override;
		virtual void RefreshLayout()override;
	};
};