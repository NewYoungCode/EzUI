#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {
	//水平滚动条
	class EZUI_API  HScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(MouseEventArgs* arg)override;
		virtual void OnMouseMove(MouseEventArgs* arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)override;
		virtual void OnParentSize(const Size& parentSize)override;
	public:
		HScrollBar(Object* ownerObject = NULL);
		virtual ~HScrollBar();

		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctrl, ScrollAlignment scrollAlignment = ScrollAlignment::Auto)override;

		//获取滑块的矩形
		virtual Rect GetSliderRect()override;
	};
};