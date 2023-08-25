#include "VScrollBar.h"
namespace EzUI {
	VScrollBar::VScrollBar() { }
	VScrollBar::~VScrollBar() { }
	void VScrollBar::RollTo(Control* ctl)
	{
		if (ctl && OWner->Parent && ctl->Parent == OWner) {
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
			__super::RollTo(rollPos, ScrollRollEventArgs(Event::None));
		}
	}
	Rect VScrollBar::GetSliderRect() {
		Rect sliderRect;
		sliderRect.X = 0;
		sliderRect.Y = (INT)_sliderPos;
		sliderRect.Width = Width();
		sliderRect.Height = _sliderLength;
		if (this->Scrollable() && sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		return sliderRect;
	}
	void VScrollBar::GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)
	{
		*viewLength = OWner->Height();
		*contentLength = OWner->GetContentSize().Height;
		*scrollBarLength = Height();
	}
	void VScrollBar::OWnerSize(const Size& size) {
		if (this->Parent == this->OWner) {
			this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
		}
	}
	void VScrollBar::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		Rect sliderRect = GetSliderRect();
		if (sliderRect.IsEmptyArea()) { return; }
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
			/*		double rate = _sliderPos / (Height() - this->_sliderLength);
					printf("%f\n", rate);*/
			__super::RollTo(_sliderPos, sbArg);
		}
	}
};