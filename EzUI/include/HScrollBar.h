#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT  HScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)override;
	public:
		HScrollBar();
		virtual ~HScrollBar();
		virtual void RollTo(Control* ctl)override;
		virtual void ParentSize(const Size& parentSize)override;
		virtual Rect GetSliderRect()override;
	};
};