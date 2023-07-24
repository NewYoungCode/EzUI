#pragma once
#include "Control.h"
#include "Timer.h"
namespace EzUI {

	class UI_EXPORT VScrollBar :
		public ScrollBar
	{
	private:
		int _maxBottom = 0;
		double sliderY = 0;
		int  pointY = 0;
		int _sliderHeight = 0;
		bool mouseDown = false;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseEnter(const Point& pt)override;
		virtual void OnMouseUp(MouseButton mBtn, const Point& point)override;
		virtual void OnMouseLeave()override;
		virtual void OnMouseMove(const Point& point)override;
		virtual void OnMouseWheel(int rollCount, short zDelta, const Point& point)override;
		virtual void OnBackgroundPaint(PaintEventArgs& e)override;
	public:
		VScrollBar();
		virtual ~VScrollBar();
		virtual void Move(double posY);
		virtual void OwnerSize(const Size& parentSize)override;
		void SetMaxBottom(int maxBottom);
		virtual Rect GetSliderRect();
		virtual bool IsDraw() override;
		virtual int RollingTotal();//
		virtual int RollingCurrent();
	};

};