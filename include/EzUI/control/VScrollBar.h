#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {
	//垂直滚动条
	class EZUI_API VScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnMouseMove(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void GetScrollMetrics(int* viewLength, int* contentLength, int* scrollBarLength)EZUI_OVERRIDE;
		virtual void OnParentSize(const Size& parentSize)EZUI_OVERRIDE;
	public:
		VScrollBar(Object* ownerObject = NULL);
		virtual ~VScrollBar();
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctrl, ScrollAlignment scrollAlignment = ScrollAlignment::Auto)EZUI_OVERRIDE;
		virtual Rect GetSliderRect()EZUI_OVERRIDE;
	};

};
