#include "VScrollBar.h"
namespace EzUI {

	VScrollBar::VScrollBar() :_maxBottom(0)
	{

	}

	VScrollBar::~VScrollBar()
	{
	}

	void VScrollBar::SetMaxBottom(int maxBottom)
	{
		this->_maxBottom = maxBottom;
		//������������
		auto& rect = GetRect();
		if (rect.Height >= _maxBottom) {
			_sliderHeight = rect.Height;
		}
		else {
			if (_maxBottom != 0) {
				//����߶�
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
		sliderRect.Y = sliderY;
		sliderRect.Height = _sliderHeight;
		return sliderRect;
	}

	void VScrollBar::OnBackgroundPaint(PaintEventArgs& e) {

		if (_sliderHeight >= Height()) {
			return;
		}
		e.Painter.FillRectangle(Rect{ 0,0,Width(),Height()}, GetBackgroundColor());
	}

	void VScrollBar::OwnerSize(const Size& size) {
		this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height});
	}

	void VScrollBar::OnSize(const Size& size)
	{
		//�˴���Ҫ����
	}
	const Rect& VScrollBar::GetRect()
	{
		//�˴���Ҫ����
		return _rect;
	}

	void VScrollBar::OnForePaint(PaintEventArgs& args)
	{
		//����rect
		Rect sliderRect(0, 0, Width(), Height());
		sliderRect.Y = sliderY;
		sliderRect.Height = _sliderHeight;
		if (_sliderHeight >= Height()) {
			return;
		}
		if (sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		//���ƻ���
		int radius = GetRadius();
		if (radius > sliderRect.Height) {
			radius = sliderRect.Height;
		}
		args.Painter.FillRectangle(sliderRect, GetForeColor(this->State), radius);
	}

	void VScrollBar::OnMouseDown(MouseButton mBtn, const Point& point) {
		__super::OnMouseDown(mBtn, point);

		Rect sliderRect(0, sliderY, Width(), _sliderHeight);
		if (_sliderHeight == Height()) { return; }
		if (mBtn == MouseButton::Left && sliderRect.Contains({ point.X,point.Y })) {
			mouseDown = true;
			this->pointY = point.Y;
		}
	}

	void VScrollBar::OnMouseEnter(const Point& pt) {
		__super::OnMouseEnter(pt);

		//{//������Ч��
		//	SetWidth(8);
		//	Style.Radius = 8;
		//	OnLayout(Size(Parent->Width(), Parent->Height()), true);
		//	Invalidate();
		//}
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

		//{//������Ч��
		//	SetWidth(5);
		//	Style.Radius = 5;
		//	OnLayout(Size(Parent->Width(), Parent->Height()), true);
		//	Invalidate();
		//}
	}

	void VScrollBar::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (mouseDown) {
			int offsetY = point.Y - this->pointY;
			sliderY += offsetY;
			pointY = point.Y;
			Move(sliderY);
			//Invalidate();//�û��϶�����ʱ�� ��Ҫ�����Ӧ�ٶ� �Ե�˿��
		}
	}

	void VScrollBar::Move(double posY) {
		sliderY = posY;
		if (sliderY <= 0) { //�����ڶ���
			sliderY = 0;
		}
		if (sliderY + _sliderHeight >= GetRect().Height) { //��������ײ�
			sliderY = GetRect().Height - _sliderHeight;
		}
		int  distanceTotal = Height() - _sliderHeight;//��ǰ������û������ܾ���
		double rate = distanceTotal * 1.0 / (_maxBottom - Parent->Height());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetY = sliderY / rate;
		if (Parent && distanceTotal > 0) {
			for (auto& it : *_controlsLocationY) { //�����ƶ�����
				it.first->SetRect({ it.first->X(), (int)(it.second - offsetY), it.first->Width(),it.first->Height() });
			}
			Parent->Invalidate();
		}
		else if (Parent) {//�������������õĵ�ʱ��
			for (auto& it : *_controlsLocationY) { //ʹ��ԭ���� �����ƶ�����
				it.first->SetRect({ it.first->X(), (int)(it.second), it.first->Width(),it.first->Height() });
			}
			Parent->Invalidate();
		}
		if (Rolling) {
			Rolling(RollingCurrent(), RollingTotal());
		}
	}


	void VScrollBar::OnMouseWheel(short zDelta, const Point& point) {
		//double offset = (Height() - GetSliderHeight())*0.01 + 0.9;
		double offset = 5;
		sliderY += (zDelta > 0 ? -offset : offset);
		Move(sliderY);
	}
};