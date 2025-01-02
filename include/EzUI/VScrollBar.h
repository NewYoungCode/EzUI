#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT VScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)override;
	public:
		VScrollBar();
		virtual ~VScrollBar();
		virtual void ScrollTo(Control* ctl)override;
		virtual void ParentSize(const Size& size)override;
		virtual Rect GetSliderRect()override;
	};

};