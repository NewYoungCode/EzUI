#include "VScrollBar.h"
namespace EzUI {
	VScrollBar::VScrollBar() { }
	VScrollBar::~VScrollBar() { }
	void VScrollBar::RefreshContent(int _contentLength)
	{
		this->_contentLength = _contentLength;
		Move(_sliderPos);
	}
	Rect VScrollBar::GetSliderRect() {
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.X = X();
		sliderRect.Y = (INT)_sliderPos;
		sliderRect.Height = _sliderLength;
		return sliderRect;
	}
	bool VScrollBar::IsDraw()
	{
		if (!CanRoll()) {
			return false;
		}
		return this->IsVisible();
	}
	bool VScrollBar::CanRoll()
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
		const Color& color = GetBackgroundColor();
		if (color.GetValue() != 0) {
			e.Graphics.SetColor(color);
			e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
		}
	}
	void VScrollBar::OwnerSize(const Size& size) {
		if (this->Parent == this->OWner) {
			this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
		}
	}
	void VScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//����rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.Y = (INT)_sliderPos;
		sliderRect.Height = _sliderLength;
		if (_sliderLength >= Height()) {
			return;
		}
		if (sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		//���ƻ���
		const Color& color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect,GetBorderTopLeftRadius());
		}
	}
	void VScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);
		Rect sliderRect(0, (INT)_sliderPos, Width(), _sliderLength);
		if (_sliderLength == Height()) { return; }
		if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			_mouseDown = true;
			this->_lastPoint = point.Y;
		}
	}
	void VScrollBar::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (_mouseDown) {
			int offsetY = point.Y - this->_lastPoint;
			_sliderPos += offsetY;
			_lastPoint = point.Y;
			Move(_sliderPos);
		}
	}
	void VScrollBar::Move(double posY) {
		if (OWner == NULL || _contentLength <= 0) {
			_sliderLength =Height();
			return;
		}
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		//����������������
		if (Height() >= _contentLength) {
			_sliderLength = Height();
		}
		else if (_contentLength) {
			//����߶�
			_sliderLength = (int)(Height() * 1.0 * Height() / _contentLength);
		}
		//����������pos
		_sliderPos = posY;
		if (_sliderPos <= 0) { //�����ڶ���
			_sliderPos = 0;
		}
		if (_sliderPos + _sliderLength >= GetRect().Height) { //��������ײ�
			_sliderPos = Height() - _sliderLength;
		}
		int distanceTotal = Height() - _sliderLength;//��ǰ������û������ܾ���
		double rate = distanceTotal * 1.0 / (_contentLength - OWner->Height());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetY = _sliderPos / rate;
		if (distanceTotal > 0) {
			int y = offsetY + 0.5;
			y = -y;
			if (OffsetCallback) {
				OffsetCallback(y);
			}
			OWner->Invalidate();
			//OWner->Refresh();//������Refresh,����������ʱ���ʱ���Ե�˿��
			if (Rolling) {
				Rolling(_sliderPos, Height() - _sliderLength);
			}
		}
	}
	void VScrollBar::OnMouseWheel(int rollCount, short zDelta, const Point& point) {
		float offset = rollCount;
		_sliderPos += (zDelta > 0 ? -offset : offset);
		Move(_sliderPos);
	}
};