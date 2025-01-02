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
	void HScrollBar::ScrollTo(Control* ctl)
	{
		if (ctl && ctl->Parent && ctl->Parent == this->Parent) {
			if (ctl->Parent->IsPendLayout()) {
				ctl->Parent->RefreshLayout();
			}
			//控件的矩形位置
			const Rect& ctlRect = ctl->GetRect();
			if (ctlRect.X >= 0 && ctlRect.GetRight() <= Width()) {
				return;
			}
			//出现在顶部
			int offset = this->_offset - ctlRect.X;
			if (ctlRect.Y > 0) {
				//出现在底部
				offset += this->_viewLength - ctlRect.Width;
			}
			__super::ScrollTo(offset, Event::None);
		}
	}
	void HScrollBar::GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)
	{
		*viewLength = this->Parent->Width();
		*contentLength = this->Parent->GetContentSize().Width;
		*scrollBarLength = Width();
	}
	void HScrollBar::ParentSize(const Size& size) {
		this->SetRect({ 0,size.Height - this->Height(),size.Width,Height() });
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
			int offset = _sliderPos * this->_rollRate + 0.5;
			__super::ScrollTo(-offset, Event::OnMouseDrag);
		}
	}
};