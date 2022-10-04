#include "HScrollBar.h"

namespace EzUI {

	HScrollBar::HScrollBar() :_maxRight(0)
	{

	}

	HScrollBar::~HScrollBar()
	{
	}

	void HScrollBar::SetMaxRight(int maxRight)
	{

		this->_maxRight = maxRight;
		//������������
		auto& rect = GetRect();
		if (rect.Width >= _maxRight) {
			_sliderWidth = rect.Width;
		}
		else {
			//����߶�
			_sliderWidth = rect.Width * 1.0 * rect.Width / _maxRight;
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
		sliderRect.X = sliderX;
		sliderRect.Y = 0;
		sliderRect.Width = _sliderWidth;
		return sliderRect;
	}

	void HScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderWidth >= Width()) {
			return;
		}
		e.Painter.FillRectangle(Rect{ 0,0,Width(),Height()}, GetBackgroundColor());
	}

	void HScrollBar::OwnerSize(const Size& parentSize) {
		this->SetRect({ 0,parentSize.Height - this->Height(),Parent->Width(),Height() });
	}

	void HScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//����rect
		Rect sliderRect(0, 0,Width(), Height());
		sliderRect.X = sliderX;
		sliderRect.Width = _sliderWidth;
		if (_sliderWidth >= Width()) {
			return;
		}
		if (sliderRect.Width <= 0) {
			sliderRect.Width = 1;
		}
		//���ƻ���
		int radius = GetRadius();
		if (radius > sliderRect.Width) {
			radius = sliderRect.Width;
		}
		args.Painter.FillRectangle(sliderRect, GetForeColor(this->State), radius);
	}

	void HScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);
		Rect sliderRect(sliderX, 0, _sliderWidth, Width());
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
			Refresh();//�û��϶�����ʱ�� ��Ҫ�����Ӧ�ٶ� �Ե�˿��
		}
	}

	void HScrollBar::Move(double posY) {
		sliderX = posY;

		if (sliderX <= 0) { //�����ڶ���
			sliderX = 0;
		}
		if (sliderX + _sliderWidth >= GetRect().Width) { //��������ײ�
			sliderX = GetRect().Width - _sliderWidth;
		}
		int  distanceTotal = Width() - _sliderWidth;//��ǰ������û������ܾ���
		double rate = distanceTotal * 1.0 / (_maxRight - Parent->Width());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetX = sliderX / rate;
		if (Parent && distanceTotal > 0) {
			for (auto& it : *_controlsLocationX) { //�����ƶ�����
				it.first->SetRect({ (int)(it.second - offsetX), it.first->Y(), it.first->Width(),it.first->Height() });
			}
			Parent->Invalidate();
		}
		else if (Parent) {//�������������õĵ�ʱ��
			for (auto& it : *_controlsLocationX) { //ʹ��ԭ����
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