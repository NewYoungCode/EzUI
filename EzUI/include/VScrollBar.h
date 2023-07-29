#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT VScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point)override;
		virtual void OnMouseWheel(int rollCount, short zDelta, const Point& point)override;
		virtual void OnBackgroundPaint(PaintEventArgs& e)override;
	public:
		VScrollBar();
		virtual ~VScrollBar();
		virtual void Move(double posY);
		virtual void OWnerSize(const Size& ownerSize)override;
		virtual Rect GetSliderRect()override;
		virtual bool IsDraw() override;
		virtual bool Scrollable()override;
	};

};