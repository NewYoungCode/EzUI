#include "HScrollBar.h"
namespace EzUI {
	HScrollBar::HScrollBar() {}
	HScrollBar::~HScrollBar() {}
	void HScrollBar::RefreshContent(int maxRight)
	{
		this->_contentLength = maxRight;
		Move(_sliderPos);
	}
	bool HScrollBar::IsDraw()
	{
		if (_sliderLength >= Width()) {
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
		e.Graphics.SetColor(GetBackgroundColor());
		e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
	}
	void HScrollBar::OwnerSize(const Size& OWnerSize) {
		if (this->Parent == this->OWner) {
			this->SetRect({ 0,OWnerSize.Height - this->Height(),OWnerSize.Width,Height() });
		}
	}
	void HScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//����rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = (INT)_sliderPos;
		sliderRect.Width = _sliderLength;
		if (_sliderLength >= Width()) {
			return;
		}
		if (sliderRect.Width <= 0) {
			sliderRect.Width = 1;
		}
		//���ƻ���
		args.Graphics.SetColor(GetForeColor());
		args.Graphics.FillRectangle(sliderRect,GetBorderTopLeftRadius());
	}
	void HScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);
		Rect sliderRect((INT)_sliderPos, 0, _sliderLength, Width());
		if (_sliderLength == Width()) { return; }
		if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			_mouseDown = true;
			this->_lastPoint = point.X;
		}
	}
	void HScrollBar::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (_mouseDown) {
			int offsetX = point.X - this->_lastPoint;
			_sliderPos += offsetX;
			_lastPoint = point.X;
			Move(_sliderPos);
		}
	}
	void HScrollBar::Move(double posY) {
		if (OWner == NULL || _contentLength <= 0) {
			_sliderLength = Width();
			return;
		}
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		//����������������
		auto& rect = GetRect();
		if (rect.Width >= _contentLength) {
			_sliderLength = rect.Width;
		}
		else {
			//����߶�
			_sliderLength = (int)(rect.Width * 1.0 * rect.Width / _contentLength);
		}
		//����������pos
		_sliderPos = posY;
		if (_sliderPos <= 0) { //�����ڶ���
			_sliderPos = 0;
		}
		if (_sliderPos + _sliderLength >= GetRect().Width) { //��������ײ�
			_sliderPos = GetRect().Width - _sliderLength;
		}
		int  distanceTotal = Width() - _sliderLength;//��ǰ������û������ܾ���
		double rate = distanceTotal * 1.0 / (_contentLength - OWner->Width());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetX = _sliderPos / rate;
		if (distanceTotal > 0) {
			int x = offsetX + 0.5;
			x = -x;
			if (OffsetCallback) {
				OffsetCallback(x);
			}
			OWner->Invalidate();
			//OWner->Refresh();//������Refresh,����������ʱ���ʱ���Ե�˿��
			if (Rolling) {
				Rolling(_sliderPos, Width() - _sliderLength);
			}
		}
	}
	void HScrollBar::OnMouseWheel(int rollCount, short zDelta, const Point& point) {
		float offset = rollCount;
		_sliderPos += (zDelta > 0 ? -offset : offset);
		Move(_sliderPos);
	}
};