#include "HScrollBar.h"
namespace EzUI {
	HScrollBar::HScrollBar() {}
	HScrollBar::~HScrollBar() {}
	Rect HScrollBar::GetSliderRect() {
		Rect sliderRect;
		sliderRect.X = (INT)_sliderPos;
		sliderRect.Y = 0;
		sliderRect.Width = _sliderLength;
		sliderRect.Height = Height();
		if (this->Scrollable() && sliderRect.Width <= 0) {
			sliderRect.Width = 1;
		}
		return sliderRect;
	}
	void HScrollBar::RollTo(Control* ctl)
	{
		if (ctl && OWner->Parent && ctl->Parent == OWner) {
			if (OWner->IsPendLayout()) {
				OWner->RefreshLayout();
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
			double rollPos = float(ctlMaxPos) / (contentLen - Width()) * rollLen;
			//滚动到控件可见的位置
			__super::RollTo(rollPos, ScrollRollEventArgs(Event::None));
		}
	}
	void HScrollBar::GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)
	{
		*viewLength = OWner->Width();
		*contentLength = OWner->GetContentSize().Width;
		*scrollBarLength = Width();
	}
	void HScrollBar::OWnerSize(const Size& OWnerSize) {
		if (this->Parent == this->OWner) {
			this->SetRect({ 0,OWnerSize.Height - this->Height(),OWnerSize.Width,Height() });
		}
	}
	void HScrollBar::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		Rect sliderRect = GetSliderRect();
		if (sliderRect.IsEmptyArea()) { return; }
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
			__super::RollTo(_sliderPos, sbArg);
		}
	}
};