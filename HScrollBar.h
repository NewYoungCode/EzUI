#pragma once
#include "Control.h"
class UI_EXPORT  HScrollBar :
	public ScrollBar
{
private:
	int _maxRight = 0;
	double sliderX = 0;
	int  pointX = 0;
	int _sliderWidth = 0;
	bool mouseDown = false;
public:
	std::map<Control*, int> *_controlsLocationX = NULL;
protected:
	virtual void OnForePaint(PaintEventArgs&args) override;
	virtual void OnMouseDown(MouseButton mBtn, const Point & point)override;
	virtual void OnMouseUp(MouseButton mBtn, const Point & point)override;
	virtual void OnMouseLeave()override;
	virtual void OnMouseMove(const Point & point)override;
	virtual void OnMouseWheel(short zDelta, const Point & point)override;
	virtual void OnBackgroundPaint(PaintEventArgs & e)override;
public:
	HScrollBar();
	virtual ~HScrollBar();
	virtual void OnLayout(const Size & sz,bool fast=true)override;
	virtual void MoveScroll(int offsetX);
	virtual Rect GetSliderRect();
	virtual void Move(double posX)override;
	void SetMaxRight(int maxRight);

	virtual int RollingTotal();//
	virtual double RollingCurrent();
};

