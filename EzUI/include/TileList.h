#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {

	class UI_EXPORT  TileList :
		public Control
	{
	private:
		int _MaxBottom = 0;
		VScrollBar vScrollBar;
		bool AutoHeight = false;//根据内容的高度自动变化
		void Init();
	public:
		TileList();
		TileList(Control*parent);
		virtual ~TileList();
		virtual void Clear(bool freeList = false)override;
		virtual void OnLayout()override;
		virtual int MoveScroll(int offset)override;
		virtual void RefreshScroll(const int& _maxBottom);
		virtual ScrollBar* GetScrollBar()override;
	};
};