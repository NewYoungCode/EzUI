#include "HScrollBar.h"

HScrollBar::HScrollBar() :_maxRight(0)
{

}

HScrollBar::~HScrollBar()
{
}

void HScrollBar::SetMaxRight(int maxRight)
{

	this->_maxRight = maxRight;
	//计算滚动条相关
	auto &rect = GetRect();
	if (rect.Width >= _maxRight) {
		_sliderWidth = rect.Width;
	}
	else {
		//滑块高度
		_sliderWidth = rect.Width*1.0*rect.Width / _maxRight;
	}
	Move(sliderX);
}

int HScrollBar::GetSliderWidth()
{
	return _sliderWidth;
}
Rect HScrollBar::GetSliderRect() {
	Rect sliderRect(0, 0, Width(), Height());
	sliderRect.X = sliderX;
	sliderRect.Y = 0;
	sliderRect.Width = _sliderWidth;
	return sliderRect;
}

void HScrollBar::OnBackgroundPaint(PaintEventArgs &e) {
	if (_sliderWidth >= _rect.Width) {
		return;
	}
	e.Painter.FillRectangle(GetBackgroundColor(), Rect{ 0,0,_rect.Width,_rect.Height });
}

void HScrollBar::OnLayout(const Size&size, bool fast) {

	this->SetRect({ 0,size.Height - this->Height(),Parent->Width(),Height() });
}

void HScrollBar::OnForePaint(PaintEventArgs&args)
{
	//滑块rect
	Rect sliderRect(0, 0, _rect.Width, _rect.Height);
	sliderRect.X = sliderX;
	sliderRect.Width = _sliderWidth;
	if (_sliderWidth >= _rect.Width) {
		return;
	}
	if (sliderRect.Width <= 0) {
		sliderRect.Width = 1;
	}
	//绘制滑块
	int radius = GetRadius();
	if (radius > sliderRect.Width) {
		radius = sliderRect.Width;
	}
	args.Painter.FillRectangle(GetForeColor(this->State), sliderRect, radius);
}

void HScrollBar::OnMouseDown(MouseButton mBtn, const Point&point) {
	__super::OnMouseDown(mBtn, point);
	Rect sliderRect(sliderX, 0, _sliderWidth, Width());
	if (_sliderWidth == Width()) { return; }
	if (mBtn == MouseButton::Left &&sliderRect.Contains({ point.X,point.Y })) {
		mouseDown = true;
		this->pointX = point.X;
	}
}
void HScrollBar::OnMouseUp(MouseButton mBtn, const Point & point)
{
	__super::OnMouseUp(mBtn, point);
	mouseDown = false;
}

void HScrollBar::OnMouseLeave()
{
	__super::OnMouseLeave();
	mouseDown = false;
}

void HScrollBar::OnMouseMove(const Point & point)
{
	__super::OnMouseMove(point);
	if (mouseDown) {
		int offsetX = point.X - this->pointX;
		sliderX += offsetX;
		pointX = point.X;
		Move(point.X);
	}
}

void HScrollBar::Move(double posY) {

	if (sliderX <= 0) { //滑块在顶部
		sliderX = 0;
	}
	if (sliderX + _sliderWidth >= GetRect().Width) { //滑块在最底部
		sliderX = GetRect().Width - _sliderWidth;
	}
	int  distanceTotal = Width() - _sliderWidth;//当前滑块可用滑道的总距离
	double rate = distanceTotal * 1.0 / (_maxRight - Parent->Width());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
	double offsetX = sliderX / rate;
	if (Parent &&distanceTotal > 0) {
		for (auto &it : *_controlsLocationX) { //挨个移动坐标
			it.first->SetRect({ (int)(it.second - offsetX), it.first->Y(), it.first->Width(),it.first->Height() });
		}
		Parent->Refresh();
	}
	else if (Parent) {//当滚动条不可用的的时候
		for (auto &it : *_controlsLocationX) { //使用原坐标
			it.first->SetRect({ (int)(it.second), it.first->Y(), it.first->Width(),it.first->Height() });
		}
		Parent->Refresh();
	}
}

void HScrollBar::MoveScroll(int offsetX)
{
	sliderX += offsetX;
	Move(sliderX);
}

void HScrollBar::OnMouseWheel(short zDelta, const Point&point) {
	//double offset = (Width() - GetSliderWidth())*0.01 + 0.9;
	double offset = 5;
	sliderX += (zDelta > 0 ? -offset : offset);
	Move(sliderX);
}

