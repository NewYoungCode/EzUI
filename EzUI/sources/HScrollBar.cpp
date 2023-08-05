#include "HScrollBar.h"
namespace EzUI {
	HScrollBar::HScrollBar() {}
	HScrollBar::~HScrollBar() {}
	bool HScrollBar::Scrollable()
	{
		if (_sliderLength >= Width()) {
			return false;
		}
		return true;
	}
	bool HScrollBar::IsDraw()
	{
		if (!Scrollable()) {
			return false;
		}
		return this->IsVisible();
	}
	Rect HScrollBar::GetSliderRect() {
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = (INT)_sliderPos;
		sliderRect.Y = 0;
		sliderRect.Width = _sliderLength;
		return sliderRect;
	}
	void HScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderLength >= Width()) {
			return;
		}
		e.Graphics.SetColor(GetBackColor());
		e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
	}
	void HScrollBar::OWnerSize(const Size& OWnerSize) {
		if (this->Parent == this->OWner) {
			this->SetRect({ 0,OWnerSize.Height - this->Height(),OWnerSize.Width,Height() });
		}
	}
	void HScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//滑块rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = (INT)_sliderPos;
		sliderRect.Width = _sliderLength;
		if (_sliderLength >= Width()) {
			return;
		}
		if (sliderRect.Width <= 0) {
			sliderRect.Width = 1;
		}
		//绘制滑块
		args.Graphics.SetColor(GetForeColor());
		args.Graphics.FillRectangle(sliderRect, GetBorderTopLeftRadius());
	}
	void HScrollBar::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		Rect sliderRect((INT)_sliderPos, 0, _sliderLength, Width());
		if (_sliderLength == Width()) { return; }
		auto point = arg.Location;
		if (arg.Button == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			_mouseDown = true;
			this->_lastPoint = point.X;
		}
	}
	void HScrollBar::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		auto point = arg.Location;
		if (_mouseDown) {
			int offsetX = point.X - this->_lastPoint;
			_sliderPos += offsetX;
			_lastPoint = point.X;
			Move(_sliderPos);
		}
	}
	void HScrollBar::Move(double posY) {
		if (OWner == NULL) return;
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		int _contentLength = OWner->GetContentSize().Width;
		if (_contentLength <= 0 || _contentLength <= OWner->Width()) {
			_sliderLength = Width();
			return;
		}
		//计算滚动条滑块相关
		auto& rect = GetRect();
		if (rect.Width >= _contentLength) {
			_sliderLength = rect.Width;
		}
		else {
			//滑块高度
			_sliderLength = (int)(rect.Width * 1.0 * rect.Width / _contentLength);
		}
		//滚动条滑块pos
		_sliderPos = posY;
		if (_sliderPos <= 0) { //滑块在顶部
			_sliderPos = 0;
		}
		if (_sliderPos + _sliderLength >= GetRect().Width) { //滑块在最底部
			_sliderPos = GetRect().Width - _sliderLength;
		}
		int  distanceTotal = Width() - _sliderLength;//当前滑块可用滑道的总距离
		double rate = distanceTotal * 1.0 / (_contentLength - OWner->Width());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
		double offsetX = _sliderPos / rate;
		if (distanceTotal > 0) {
			int x = offsetX + 0.5;
			x = -x;
			if (OffsetCallback) {
				OffsetCallback(x);
			}
			OWner->Invalidate();
			//OWner->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
			if (Rolling) {
				Rolling(_sliderPos, Width() - _sliderLength);
			}
		}
	}
	void HScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		auto offset = arg.RollCount;
		_sliderPos += (arg.ZDelta > 0 ? -offset : offset);
		Move(_sliderPos);
	}
};