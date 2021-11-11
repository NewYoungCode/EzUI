#pragma once
#include "Control.h"
class UI_EXPORT VScrollBar :
	public ScrollBar
{
private:
	int _maxBottom = 0;
	double sliderY = 0;
	int  pointY = 0;
	int _sliderHeight = 0;
	bool mouseDown = false;
public:
	std::map<Control*, int> *_controlsLocationY = NULL;
protected:
	virtual void OnForePaint(PaintEventArgs&args) override;
	virtual void OnMouseDown(MouseButton mBtn, const Point & point)override;
	virtual void OnMouseUp(MouseButton mBtn, const Point & point)override;
	virtual void OnMouseLeave()override;
	virtual void OnMouseMove(const Point & point)override;
	virtual void OnMouseWheel(short zDelta, const Point & point)override;
	virtual void OnBackgroundPaint(PaintEventArgs & e)override;
public:
	VScrollBar();
	virtual ~VScrollBar();
	virtual void Move(double posY);
	virtual void OnLayout(const Size & sz, bool fast = true)override;
	virtual void MoveScroll(int offsetY);
	void SetMaxBottom(int maxBottom);
	virtual Rect GetSliderRect();

	virtual int RollingTotal();//
	virtual double RollingCurrent();
};

