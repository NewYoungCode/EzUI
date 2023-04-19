#include "HScrollBar.h"

namespace EzUI {

	HScrollBar::HScrollBar() :_maxRight(0) {}
	HScrollBar::~HScrollBar() {}

	void HScrollBar::SetMaxRight(int maxRight)
	{

		this->_maxRight = maxRight;
		//计算滚动条相关
		auto& rect = GetRect();
		if (rect.Width >= _maxRight) {
			_sliderWidth = rect.Width;
		}
		else {
			//滑块高度
			_sliderWidth = (int)(rect.Width * 1.0 * rect.Width / _maxRight);
		}
		Move(sliderX);
	}

	int HScrollBar::RollingTotal()
	{
		auto sliderRect = GetSliderRect();
		return Width() - _sliderWidth;
	}

	int HScrollBar::RollingCurrent()
	{
		return (int)sliderX;
	}

	Rect HScrollBar::GetSliderRect() {
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = (INT)sliderX;
		sliderRect.Y = 0;
		sliderRect.Width = _sliderWidth;
		return sliderRect;
	}

	void HScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderWidth >= Width()) {
			return;
		}
		EzUI::FillRectangle(e.Painter, Rect{ 0,0,Width(),Height() }, GetBackgroundColor());
	}

	void HScrollBar::OwnerSize(const Size& parentSize) {
		this->SetRect({ 0,parentSize.Height - this->Height(),parentSize.Width,Height() });
	}

	void HScrollBar::OnSize(const Size& size)
	{
		//此处需要屏蔽
	}
	const Rect& HScrollBar::GetRect()
	{
		//此处需要屏蔽
		return _rect;
	}
	void HScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//滑块rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = (INT)sliderX;
		sliderRect.Width = _sliderWidth;
		if (_sliderWidth >= Width()) {
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
		EzUI::FillRectangle(args.Painter, sliderRect, GetForeColor(this->State), radius);
	}

	void HScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);
		Rect sliderRect((INT)sliderX, 0, _sliderWidth, Width());
		if (_sliderWidth == Width()) { return; }
		if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			mouseDown = true;
			this->pointX = point.X;
		}
	}
	void HScrollBar::OnMouseUp(MouseButton mBtn, const Point& point)
	{
		__super::OnMouseUp(mBtn, point);
		mouseDown = false;
	}

	void HScrollBar::OnMouseLeave()
	{
		__super::OnMouseLeave();
		mouseDown = false;
	}

	void HScrollBar::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (mouseDown) {
			int offsetX = point.X - this->pointX;
			sliderX += offsetX;
			pointX = point.X;
			Move(sliderX);
			//Invalidate();//用户拖动鼠标的时候 需要提高响应速度 显得丝滑
		}
	}

	void HScrollBar::Move(double posY) {
		sliderX = posY;

		if (sliderX <= 0) { //滑块在顶部
			sliderX = 0;
		}
		if (sliderX + _sliderWidth >= GetRect().Width) { //滑块在最底部
			sliderX = GetRect().Width - _sliderWidth;
		}
		int  distanceTotal = Width() - _sliderWidth;//当前滑块可用滑道的总距离
		double rate = distanceTotal * 1.0 / (_maxRight - Parent->Width());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
		double offsetX = sliderX / rate;
		if (Parent && distanceTotal > 0) {
			for (auto& it : this->Location) { //挨个移动坐标
				it.first->SetRect({ (int)(it.second - offsetX), it.first->Y(), it.first->Width(),it.first->Height() });
			}
			Parent->Invalidate();
		}
		else if (Parent) {//当滚动条不可用的的时候
			for (auto& it : this->Location) { //使用原坐标
				it.first->SetRect({ (int)(it.second), it.first->Y(), it.first->Width(),it.first->Height() });
			}
			Parent->Invalidate();
		}
		if (Rolling) {
			Rolling(RollingCurrent(), RollingTotal());
		}
	}
	void HScrollBar::OnMouseWheel(short zDelta, const Point& point) {
		//double offset = (Width() - GetSliderWidth())*0.01 + 0.9;
		double offset = 5;
		sliderX += (zDelta > 0 ? -offset : offset);
		Move(sliderX);
	}
};