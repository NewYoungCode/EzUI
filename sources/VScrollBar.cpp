#include "VScrollBar.h"
namespace EzUI {
	VScrollBar::VScrollBar() { }
	VScrollBar::~VScrollBar() { }
	void VScrollBar::ScrollTo(Control* ctl)
	{
		if (ctl && ctl->Parent && ctl->Parent == this->Parent) {
			if (ctl->Parent->IsPendLayout()) {
				ctl->Parent->RefreshLayout();
			}
			//控件的矩形位置
			const Rect& ctlRect = ctl->GetRect();
			if (ctlRect.Y >= 0 && ctlRect.GetBottom() <= Height()) {
				return;
			}
			//出现在顶部
			int offset = this->_offset - ctlRect.Y;
			if (ctlRect.Y > 0) {
				//出现在底部
				offset += this->_viewLength - ctlRect.Height;
			}
			__super::ScrollTo(offset, Event::None);
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
		*viewLength = this->Parent->Height();
		*contentLength = this->Parent->GetContentSize().Height;
		*scrollBarLength = Height();
	}
	void VScrollBar::ParentSize(const Size& size) {
		this->SetRect({ size.Width - this->Width() ,0,this->Width(),size.Height });
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
			int offset = _sliderPos * this->_rollRate + 0.5;
			__super::ScrollTo(-offset, Event::OnMouseDrag);
		}
	}
};