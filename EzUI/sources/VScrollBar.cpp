#include "VScrollBar.h"
namespace EzUI {
	VScrollBar::VScrollBar() { }
	VScrollBar::~VScrollBar() { }
	void VScrollBar::RollTo(Control* ctl)
	{
		if (OWner->Parent && ctl->Parent == OWner) {
			if (OWner->IsPendLayout()) {
				OWner->ResumeLayout();
			}
			//�ؼ��ľ���λ��
			const Rect& ctlRect = ctl->GetRect();
			if (ctlRect.Y >= 0 && ctlRect.GetBottom() <= Height()) {
				return;
			}
			//�������ɹ����ĳ���
			int rollLen = Height() - _sliderLength;
			//�������������ܸ߶�
			int contentLen = OWner->GetContentSize().Height;
			//������ؼ�Ӧ������λ��
			int ctlMaxPos = -this->_offset + ctlRect.GetBottom() - OWner->Height() / 2.0f;
			//�����������Ӧ�ù�������λ��
			float rollPos = float(ctlMaxPos) / (contentLen - Height()) * rollLen;
			//�������ؼ��ɼ���λ��
			this->RollTo(rollPos, ScrollRollEventArgs(Event::None));
		}
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
			ScrollRollEventArgs sbArg;
			sbArg.RollType = Event::OnMouseDrag;
			sbArg.ZDelta = -offsetY;
			RollTo(_sliderPos, sbArg);
		}
	}
	void VScrollBar::RollTo(double posY, const  ScrollRollEventArgs& args) {
		if (OWner == NULL) return;
		if (OWner->IsPendLayout()) {
			OWner->ResumeLayout();
		}
		int _contentLength = OWner->GetContentSize().Height;
		//int overflowLen = _contentLength - OWner->Height();
		if (_contentLength <= 0 || _contentLength <= OWner->Height()) {
			_sliderLength = Height();
			return;
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
			this->_offset = y;
			if (OffsetCallback) {
				OffsetCallback(y);
			}
			OWner->Invalidate();
			//OWner->Refresh();//������Refresh,����������ʱ���ʱ���Ե�˿��
			if (Rolling) {
				((ScrollRollEventArgs&)args).Pos = _sliderPos;
				((ScrollRollEventArgs&)args).Total = Height() - _sliderLength;
				Rolling(this, args);
			}
		}
	}
	void VScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
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