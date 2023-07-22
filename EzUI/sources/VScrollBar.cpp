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
		Color color = GetBackgroundColor();
		if (color.GetValue() != 0) {
			e.Graphics.SetColor(color);
			e.Graphics.FillRectangle(Rect{ 0,0,Width(),Height() });
		}
	}

	void VScrollBar::OwnerSize(const Size& size) {
		this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
	}

	void VScrollBar::OnSize(const Size& size)
	{
		//�˴���Ҫ����
	}
	const Rect& VScrollBar::GetRect()
	{
		//�˴���Ҫ����
		return __super::GetRect();
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
		Color color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, GetRadius());
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
		Parent->ResumeLayout();
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
		double rate = distanceTotal * 1.0 / (_maxBottom - Parent->Height());//��������ܸ߶� / list item�߶��ܺ� * ��ǰ�������������
		double offsetY = sliderY / rate;
		if (distanceTotal > 0) {
			int y = offsetY + 0.5;
			y = -y;
			Parent->MoveScroll(y);
			Parent->Invalidate();
			//Parent->Refresh();//������Refresh,����������ʱ���ʱ���Ե�˿��
			if (Rolling) {
				Rolling(RollingCurrent(), RollingTotal());
			}
		}

	}

	void VScrollBar::OnMouseWheel(short zDelta, const Point& point) {
		int fx = (zDelta > 0 ? -5 : 5);//��������
		if (fx != _speed) {//��������������ϴβ�ͬ ����ֹͣ
			_rollCount = 1;//����һ��
		}
		else {
			_rollCount += 3;//���������
		}
		_speed = fx;//���Ʒ������ٶ�
		_timer.Start();
	}
};