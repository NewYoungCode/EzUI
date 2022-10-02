#pragma once
#include "Layout.h"
#include "VScrollBar.h"
namespace EzUI {

	class UI_EXPORT  TileList :
		public Control,public  IScroll
	{
	private:
		int _MaxBottom = 0;
		VScrollBar* vScrollBar = NULL;
	public:
		TileList();
		virtual ~TileList();
		virtual void Clear(bool freeList = false)override;
		virtual bool OnSize(const Size& sz) override;
		virtual void ResumeLayout()override;
	};
};