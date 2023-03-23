#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {

	class UI_EXPORT  TileList :
		public Control,public  IScroll
	{
		UI_GetClassName()
	private:
		int _MaxBottom = 0;
		VScrollBar* vScrollBar = NULL;
		bool AutoHeight = false;//根据内容的高度自动变化
	public:
		TileList();
		virtual ~TileList();
		virtual void Clear(bool freeList = false)override;
		virtual void OnSize(const Size& sz) override;
		virtual void OnLayout()override;
		virtual void ResumeLayout() override;
		virtual void RefreshScroll(const int& _maxBottom);
	};
};