#include "VScrollBar.h"
namespace EzUI {
	VScrollBar::VScrollBar() { }
	VScrollBar::~VScrollBar() { }
	Rect VScrollBar::GetSliderRect() {
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = X();
		sliderRect.Y = (INT)_sliderPos;
		sliderRect.Height = _sliderLength;
		return sliderRect;
	}
	bool VScrollBar::IsDraw()
	{
		if (!Scrollable()) {
			return false;
		}
		return this->IsVisible();
	}
	bool VScrollBar::Scrollable()
	{
		if (_sliderLength >= Height()) {
			return false;
		}
		return true;
	}
	void VScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderLength >= Height()) {
			return;
		}
		const Color& color = GetBackColor();
		if (color.GetValue() != 0) {
			e.Graphics.SetColor(color);
			e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
		}
	}
	void VScrollBar::OWnerSize(const Size& size) {
		if (this->Parent == this->OWner) {
			this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
		}
	}
	void VScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//滑块rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.Y = (INT)_sliderPos;
		sliderRect.Height = _sliderLength;
		if (_sliderLength >= Height()) {
			return;
		}
		if (sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		//绘制滑块
		const Color& color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, GetBorderTopLeftRadius());
		}
	}
	void VScrollBar::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		Rect sliderRect(0, (INT)_sliderPos, Width(), _sliderLength);
		if (_sliderLength == Height()) { return; }
		auto point = arg.Location;
		if (arg.Button == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			_mouseDown = true;
			this->_lastPoint = point.Y;
		}
	}
	void VScrollBar::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		if (_mouseDown) {
			auto point = arg.Location;
			int offsetY = point.Y - this->_lastPoint;
			_sliderPos += offsetY;
			_lastPoint = point.Y;
			Move(_sliderPos);
		}
	}
	void VScrollBar::Move(double posY) {
		if (OWner == NULL) return;
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		int _contentLength = OWner->GetContentSize().Height;
		if (_contentLength <= 0 || _contentLength <= OWner->Height()) {
			_sliderLength = Height();
			return;
		}
		//计算滚动条滑块相关
		if (Height() >= _contentLength) {
			_sliderLength = Height();
		}
		else if (_contentLength) {
			//滑块高度
			_sliderLength = (int)(Height() * 1.0 * Height() / _contentLength);
		}
		//滚动条滑块pos
		_sliderPos = posY;
		if (_sliderPos <= 0) { //滑块在顶部
			_sliderPos = 0;
		}
		if (_sliderPos + _sliderLength >= GetRect().Height) { //滑块在最底部
			_sliderPos = Height() - _sliderLength;
		}
		int distanceTotal = Height() - _sliderLength;//当前滑块可用滑道的总距离
		double rate = distanceTotal * 1.0 / (_contentLength - OWner->Height());//滑块可用总高度 / list item高度总和 * 当前滑块坐标的坐标
		double offsetY = _sliderPos / rate;
		if (distanceTotal > 0) {
			int y = offsetY + 0.5;
			y = -y;
			if (OffsetCallback) {
				OffsetCallback(y);
			}
			OWner->Invalidate();
			//OWner->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
			if (Rolling) {
				Rolling(_sliderPos, Height() - _sliderLength);
			}
		}
	}
	void VScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		auto offset = arg.RollCount;
		_sliderPos += (arg.ZDelta > 0 ? -offset : offset);
		Move(_sliderPos);
	}
};