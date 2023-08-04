#pragma once
#include "Control.h"
#include "HScrollBar.h"
namespace EzUI {
	class UI_EXPORT  HList :
		public ScrollableControl
	{
	private:
		HScrollBar hScrollBar;
		void Init();
		int Offset(int offset);
	public:
		HList();
		virtual ~HList();
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual ScrollBar* GetScrollBar()override;
		
	};
};