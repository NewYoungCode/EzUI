#include "HScrollBar.h"

namespace ezui {
	HScrollBar::HScrollBar(Object* parentObj):ScrollBar(parentObj)
	{
	}
	HScrollBar::~HScrollBar() {}
	Rect HScrollBar::GetSliderRect() {
		Rect sliderRect;
		sliderRect.X = (int_t)m_sliderPos;
		sliderRect.Y = 0;
		sliderRect.Width = m_sliderLength;
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
			int_t offset = this->m_offset - ctlRect.X;
			if (ctlRect.Y > 0) {
				//出现在底部
				offset += this->m_viewLength - ctlRect.Width;
			}
			__super::ScrollTo(offset, Event::None);
		}
	}
	void HScrollBar::GetInfo(int_t* viewLength, int_t* contentLength, int_t* scrollBarLength)
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
			m_mouseDown = true;
			this->m_lastPoint = point.X;
		}
	}
	void HScrollBar::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		auto point = arg.Location;
		if (m_mouseDown) {
			int_t offsetX = point.X - this->m_lastPoint;
			m_sliderPos += offsetX;
			m_lastPoint = point.X;
			int_t offset = m_sliderPos * this->m_rollRate + 0.5;
			__super::ScrollTo(-offset, Event::OnMouseDrag);
		}
	}
};