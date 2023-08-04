#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public ScrollableControl
	{
	private:
		VScrollBar vScrollBar;
		void Init();
		//对控件进行偏移
		int Offset(int offset);
	public:
		VList();
		virtual ~VList();
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual ScrollBar* GetScrollBar() override;
	};
};