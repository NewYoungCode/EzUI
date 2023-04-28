#include "VScrollBar.h"
namespace EzUI {

	VScrollBar::VScrollBar() {
		_timer.Interval = 5;
		_timer.Tick = [this](Windows::Timer* tm)->void {
			sliderY += _speed;
			Move(sliderY);
			_rollCount--;
			if (_rollCount <= 0) {
				tm->Stop();
			}
		};
	}
	VScrollBar::~VScrollBar() {
		_timer.Stop();
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
				_sliderHeight = (int)(rect.Height * 1.0 * rect.Height / _maxBottom);
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
		sliderRect.Y = (INT)sliderY;
		sliderRect.Height = _sliderHeight;
		return sliderRect;
	}

	void VScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderHeight >= Height()) {
			return;
		}
		Color color = GetBackgroundColor();
		if (color.GetValue() != 0) {
			e.Graphics.SetColor(color);
			e.Graphics.FillRectangle( Rect{ 0,0,Width(),Height() });
		}
	}

	void VScrollBar::OwnerSize(const Size& size) {
		this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
	}

	void VScrollBar::OnSize(const Size& size)
	{
		//此处需要屏蔽
	}
	const Rect& VScrollBar::GetRect()
	{
		//此处需要屏蔽
		return _rect;
	}

	void VScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//滑块rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.Y = (INT)sliderY;
		sliderRect.Height = _sliderHeight;
		if (_sliderHeight >= Height()) {
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
		Color color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, radius);
		}
	}

	void VScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);
		Rect sliderRect(0, (INT)sliderY, Width(), _sliderHeight);
		if (_sliderHeight == Height()) { return; }
		if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			mouseDown = true;
			this->pointY = point.Y;
		}
	}

	void VScrollBar::OnMouseEnter(const Point& pt) {
		__super::OnMouseEnter(pt);
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
		if (Parent == NULL) {
			return;
		}
		sliderY = posY;
		if (sliderY <= 0) { //滑块在顶部
			sliderY = 0;
		}
		if (sliderY + _sliderHeight >= GetRect().Height) { //滑块在最底部
			sliderY = GetRect().Height - _sliderHeight;
		}
		int distanceTotal = Height() - _sliderHeight;//当前滑块可用滑道的总距离
		double rate = distanceTotal * 1.0 / (_maxBottom - Parent->Height());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
		double offsetY = sliderY / rate;
		if (distanceTotal > 0) {
			for (auto& it : this->Location) { //挨个移动坐标
				it.first->SetRect({ it.first->X(), (int)(it.second - offsetY), it.first->Width(),it.first->Height() });
			}
		}
		else {//当滚动条不可用的的时候
			for (auto& it : this->Location) { //使用原坐标 挨个移动坐标
				it.first->SetRect({ it.first->X(), (int)(it.second), it.first->Width(),it.first->Height() });
			}
		}
		Parent->Invalidate();
		//Parent->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
		if (Rolling) {
			Rolling(RollingCurrent(), RollingTotal());
		}
	}

	void VScrollBar::OnMouseWheel(short zDelta, const Point& point) {
		int fx = (zDelta > 0 ? -5 : 5);//滚动方向
		if (fx != _speed) {//如果滚动方向与上次不同 即可停止
			_rollCount = 1;//滚动一次
		}
		else {
			_rollCount += 5;//将滚动五次
		}
		_speed = fx;//控制方向与速度
		_timer.Start();
	}
};