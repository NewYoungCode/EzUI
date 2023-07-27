#pragma once
#include "Control.h"
#include "Timer.h"
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
		virtual void OwnerSize(const Size& ownerSize)override;
		virtual void RefreshContent(int _contentLength)override;
		virtual Rect GetSliderRect();
		virtual bool IsDraw() override;
		virtual bool CanRoll()override;
	};

};