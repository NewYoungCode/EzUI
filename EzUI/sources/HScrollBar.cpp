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
	void HScrollBar::RollTo(Control* ctl)
	{
		if (OWner->Parent && ctl->Parent == OWner) {
			if (OWner->IsPendLayout()) {
				OWner->ResumeLayout();
			}
			//控件的矩形位置
			const Rect& ctlRect = ctl->GetRect();
			if (ctlRect.X >= 0 && ctlRect.GetRight() <= Width()) {
				return;
			}
			//滚动条可滚动的长度
			int rollLen = Width() - _sliderLength;
			//容器内上下文总高度
			int contentLen = OWner->GetContentSize().Width;
			//计算出控件应该虚拟位置
			int ctlMaxPos = -this->_offset + ctlRect.GetRight() - OWner->Width() / 2.0f;
			//计算出滚动条应该滚动到的位置
			float rollPos = float(ctlMaxPos) / (contentLen - Width()) * rollLen;
			//滚动到控件可见的位置
			this->RollTo(rollPos, ScrollRollEventArgs(Event::None));
		}
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
			ScrollRollEventArgs sbArg;
			sbArg.RollType = Event::OnMouseDrag;
			sbArg.ZDelta = -offsetX;
			RollTo(_sliderPos, sbArg);
		}
	}
	void HScrollBar::RollTo(double posY, const  ScrollRollEventArgs& args) {
		if (OWner == NULL) return;
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		int _contentLength = OWner->GetContentSize().Width;
		//int overflowLen = _contentLength - OWner->Width();
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
			this->_offset = x;
			if (OffsetCallback) {
				OffsetCallback(x);
			}
			OWner->Invalidate();
			//OWner->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
			if (Rolling) {
				((ScrollRollEventArgs&)args).Pos = _sliderPos;
				((ScrollRollEventArgs&)args).Total = Width() - _sliderLength;
				Rolling(this, args);
			}
		}
	}
	void HScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		auto offset = arg.RollCount;
		_sliderPos += (arg.ZDelta > 0 ? -offset : offset);
		ScrollRollEventArgs args;
		args.RollType = Event::OnMouseWheel;
		args.ZDelta = arg.ZDelta;
		args.Speed = arg.RollCount;
		RollTo(_sliderPos, args);
	}
};