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
		virtual void OnParentSize(const Size& parentSize)override;
	public:
		HScrollBar(Object* ownerObject = NULL);
		virtual ~HScrollBar();
		virtual void ScrollTo(Control* ctl)override;
		virtual Rect GetSliderRect()override;
	};
};