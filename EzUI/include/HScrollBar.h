#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT  HScrollBar :
		public ScrollBar
	{
	private:
		int _maxRight = 0;
		double sliderX = 0;
		int  pointX = 0;
		int _sliderWidth = 0;
		bool mouseDown = false;
	protected:
		virtual void OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseUp(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseLeave()override;
		virtual void OnMouseMove(const Point& point)override;
		virtual void OnMouseWheel(short zDelta, const Point& point)override;
		virtual void OnBackgroundPaint(PaintEventArgs& e)override;
		virtual const Rect& GetRect() override;
	public:
		HScrollBar();
		virtual ~HScrollBar();
		virtual void OwnerSize(const Size& parentSize)override;
		virtual Rect GetSliderRect();
		virtual void Move(double posX)override;
		void SetMaxRight(int maxRight);
		virtual bool IsDraw()override;
		virtual int RollingTotal();//
		virtual int RollingCurrent();
	};

};