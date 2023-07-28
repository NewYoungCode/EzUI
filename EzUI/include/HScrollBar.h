#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT  HScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point)override;
		virtual void OnMouseWheel(int rollCount, short zDelta, const Point& point)override;
		virtual void OnBackgroundPaint(PaintEventArgs& e)override;
	public:
		HScrollBar();
		virtual ~HScrollBar();
		virtual void OwnerSize(const Size& parentSize)override;
		virtual Rect GetSliderRect()override;
		virtual void Move(double posX)override;
		virtual bool IsDraw()override;
		virtual void RefreshContent(int maxContent)override;
		virtual bool CanRoll()override;
	};
};