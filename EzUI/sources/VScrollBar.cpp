#include "VScrollBar.h"
namespace EzUI {

	VScrollBar::VScrollBar() {
	}
	VScrollBar::~VScrollBar() {
	}
	void VScrollBar::SetMaxBottom(int maxBottom)
	{
		this->_maxBottom = maxBottom;
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


	bool VScrollBar::IsDraw()
	{
		if (_sliderHeight >= Height()) {
			return false;
		}
		return this->IsVisible();
	}

	void VScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (_sliderHeight >= Height()) {
			return;
		}
		const Color& color = e.Style.BackgroundColor;
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
		sliderRect.Y = (INT)sliderY;
		sliderRect.Height = _sliderHeight;
		if (_sliderHeight >= Height()) {
			return;
		}
		if (sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		//���ƻ���
		const Color& color = args.Style.ForeColor;
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, args.Style.Border.TopLeftRadius);
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
		if (OWner == NULL) {
			return;
		}
		OWner->ResumeLayout();
		sliderY = posY;
		if (sliderY <= 0) { //�����ڶ���
			sliderY = 0;
		}
		//������������
		if (Height() >= _maxBottom) {
			_sliderHeight = Height();
		}
		else if (_maxBottom != 0) {
			//����߶�
			_sliderHeight = (int)(Height() * 1.0 * Height() / _maxBottom);
		}
		if (sliderY + _sliderHeight >= GetRect().Height) { //��������ײ�
			sliderY = Height() - _sliderHeight;
		}
		int distanceTotal = Height() - _sliderHeight;//��ǰ������û������ܾ���
		double rate = distanceTotal * 1.0 / (_maxBottom - OWner->Height());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetY = sliderY / rate;
		if (distanceTotal > 0) {
			int y = offsetY + 0.5;
			y = -y;
			OWner->MoveScroll(y);
			OWner->Invalidate();
			//OWner->Refresh();//������Refresh,����������ʱ���ʱ���Ե�˿��
			if (Rolling) {
				Rolling(RollingCurrent(), RollingTotal());
			}
		}
	}

	void VScrollBar::OnMouseWheel(int rollCount, short zDelta, const Point& point) {
		float offset = rollCount;
		sliderY += (zDelta > 0 ? -offset : offset);
		Move(sliderY);
	}
};