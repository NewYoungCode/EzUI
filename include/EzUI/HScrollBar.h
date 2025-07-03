#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {

	class UI_EXPORT  HScrollBar :
		public IScrollBar
	{
	protected:
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg)override;
		virtual void GetInfo(int_t* viewLength, int_t* contentLength, int_t* scrollBarLength)override;
	public:
		HScrollBar();
		virtual ~HScrollBar();
		virtual void ScrollTo(Control* ctl)override;
		virtual void ParentSize(const Size& parentSize)override;
		virtual Rect GetSliderRect()override;
	};
};