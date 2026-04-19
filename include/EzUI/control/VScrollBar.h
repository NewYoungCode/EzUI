#pragma once
#include "Control.h"
#include "ScrollBar.h"

namespace ezui {
	//垂直滚动条
	class EZUI_API VScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnMouseDown(MouseEventArgs* arg)override;
		virtual void OnMouseMove(MouseEventArgs* arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)override;
		virtual void OnParentSize(const Size& parentSize)override;
	public:
		VScrollBar(Object* ownerObject = NULL);
		virtual ~VScrollBar();
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctrl, ScrollAlignment scrollAlignment = ScrollAlignment::Auto)override;
		virtual Rect GetSliderRect()override;
	};

};