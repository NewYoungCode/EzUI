#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {

	class UI_EXPORT  HScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)override;
	public:
		HScrollBar(Object* parentObject = NULL);
		virtual ~HScrollBar();
		virtual void ScrollTo(Control* ctl)override;
		virtual void ParentSize(const Size& parentSize)override;
		virtual Rect GetSliderRect()override;
	};
};