#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT VScrollBar :
		public ScrollBar
	{
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg)override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
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