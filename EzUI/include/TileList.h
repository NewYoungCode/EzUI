#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {

	class UI_EXPORT  TileList :
		public ScrollableControl
	{
	private:
		VScrollBar vScrollBar;
		bool AutoHeight = false;//根据内容的高度自动变化
		void Init();
		 int Offset(int offset);
	public:
		TileList();
		virtual ~TileList();
		virtual void Clear(bool freeList = false)override;
		virtual void OnLayout()override;
		virtual ScrollBar* GetScrollBar()override;
	};
};