#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {
	//水平滚动条
	class EZUI_API  HScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnMouseMove(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void GetScrollMetrics(int* viewLength, int* contentLength, int* scrollBarLength)EZUI_OVERRIDE;
		virtual void OnParentSize(const Size& parentSize)EZUI_OVERRIDE;
	public:
		HScrollBar(Object* ownerObject = NULL);
		virtual ~HScrollBar();

		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctrl, ScrollAlignment scrollAlignment = ScrollAlignment::Auto)EZUI_OVERRIDE;

		//获取滑块的矩形
		virtual Rect GetSliderRect()EZUI_OVERRIDE;
	};
};
