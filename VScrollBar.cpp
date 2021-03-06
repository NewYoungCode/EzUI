#include "VScrollBar.h"

VScrollBar::VScrollBar() :_maxBottom(0)
{

}

VScrollBar::~VScrollBar()
{
}

void VScrollBar::SetMaxBottom(int maxBottom)
{
	this->_maxBottom = maxBottom;
	//计算滚动条相关
	auto& rect = GetRect();
	if (rect.Height >= _maxBottom) {
		_sliderHeight = rect.Height;
	}
	else {
		if (_maxBottom != 0) {
			//滑块高度
			_sliderHeight = rect.Height * 1.0 * rect.Height / _maxBottom;
		}
	}
	Move(sliderY);
}
int VScrollBar::RollingTotal()
{
	auto sliderRect = GetSliderRect();
	return Height() - _sliderHeight;
}

int VScrollBar::RollingCurrent()
{
	return (int)sliderY;
}


Rect VScrollBar::GetSliderRect() {

	Rect sliderRect(0, 0, Width(), Height());
	sliderRect.X = X();
	sliderRect.Y = sliderY;
	sliderRect.Height = _sliderHeight;
	return sliderRect;
}

void VScrollBar::OnBackgroundPaint(PaintEventArgs& e) {

	if (_sliderHeight >= _rect.Height) {
		return;
	}
	e.Painter.FillRectangle(GetBackgroundColor(), Rect{ 0,0,_rect.Width,_rect.Height });
}

void VScrollBar::OnLayout(const Size& size, bool fast) {

	this->SetRect({ size.Width - this->Width() ,0,this->GetRect().Width,Parent->Height() });
}

void VScrollBar::OnForePaint(PaintEventArgs& args)
{
	//滑块rect
	Rect sliderRect(0, 0, _rect.Width, _rect.Height);
	sliderRect.Y = sliderY;
	sliderRect.Height = _sliderHeight;
	if (_sliderHeight >= _rect.Height) {
		return;
	}
	if (sliderRect.Height <= 0) {
		sliderRect.Height = 1;
	}
	//绘制滑块
	int radius = GetRadius();
	if (radius > sliderRect.Height) {
		radius = sliderRect.Height;
	}
	args.Painter.FillRectangle(GetForeColor(this->State), sliderRect, radius);
}

void VScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
	__super::OnMouseDown(mBtn, point);

	Rect sliderRect(0, sliderY, Width(), _sliderHeight);
	if (_sliderHeight == Height()) { return; }
	if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
		mouseDown = true;
		this->pointY = point.Y;
	}
}
void VScrollBar::OnMouseUp(MouseButton mBtn, const Point& point)
{
	__super::OnMouseUp(mBtn, point);
	mouseDown = false;
}

void VScrollBar::OnMouseLeave()
{
	__super::OnMouseLeave();
	mouseDown = false;
}

void VScrollBar::OnMouseMove(const Point& point)
{
	__super::OnMouseMove(point);
	if (mouseDown) {
		int offsetY = point.Y - this->pointY;
		sliderY += offsetY;
		pointY = point.Y;
		Move(sliderY);
	}
}

void VScrollBar::Move(double posY) {
	sliderY = posY;

	if (sliderY <= 0) { //滑块在顶部
		sliderY = 0;
	}
	if (sliderY + _sliderHeight >= GetRect().Height) { //滑块在最底部
		sliderY = GetRect().Height - _sliderHeight;
	}
	int  distanceTotal = Height() - _sliderHeight;//当前滑块可用滑道的总距离
	double rate = distanceTotal * 1.0 / (_maxBottom - Parent->Height());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
	double offsetY = sliderY / rate;
	if (Parent && distanceTotal > 0) {
		for (auto& it : *_controlsLocationY) { //挨个移动坐标
			it.first->SetRect({ it.first->X(), (int)(it.second - offsetY), it.first->Width(),it.first->Height() });
		}
		Parent->Refresh();
	}
	else if (Parent) {//当滚动条不可用的的时候
		for (auto& it : *_controlsLocationY) { //使用原坐标 挨个移动坐标
			it.first->SetRect({ it.first->X(), (int)(it.second), it.first->Width(),it.first->Height() });
		}
		Parent->Refresh();
	}
	if (Rolling) {
		Rolling(RollingCurrent(), RollingTotal());
	}
}


void VScrollBar::OnMouseWheel(short zDelta, const Point& point) {
	//double offset = (Height() - GetSliderHeight())*0.01 + 0.9;
	double offset = 5;
	sliderY += (zDelta > 0 ? -offset : offset);
	Move(sliderY);
}
