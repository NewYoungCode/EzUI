#include "HScrollBar.h"

namespace EzUI {

	HScrollBar::HScrollBar() {}
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
		e.Graphics.SetColor(GetBackgroundColor());
		e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
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

		args.Graphics.SetColor(GetForeColor(this->State));
		args.Graphics.FillRectangle(sliderRect, radius);
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
		}
	}

	void HScrollBar::Move(double posY) {
		if (Parent == NULL) {
			return;
		}
		Parent->ResumeLayout();
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
		if (distanceTotal > 0) {
			int x = -offsetX + 0.5;
			//x = -x;
			for (auto& it : Parent->GetControls()) { //挨个移动坐标
				if (it->Visible == false) {
					continue;
				}
				x += it->Margin.Left;
				it->SetRect({ x, it->Y(), it->Width(),it->Height() });
				x += it->Margin.Right;
				x += it->Width();
			}
			Parent->Invalidate();
			//Parent->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
			if (Rolling) {
				Rolling(RollingCurrent(), RollingTotal());
			}
		}

	}
	void HScrollBar::OnMouseWheel(short zDelta, const Point& point) {
		//double offset = (Width() - GetSliderWidth())*0.01 + 0.9;
		double offset = 5;
		sliderX += (zDelta > 0 ? -offset : offset);
		Move(sliderX);
	}
};