#include "VScrollBar.h"
namespace ezui {
	VScrollBar::VScrollBar(Object* ownerObject):ScrollBar(ownerObject)
	{
	}
	VScrollBar::~VScrollBar() { }
	void VScrollBar::ScrollTo(Control* ctl)
	{
		if (ctl && ctl->GetParent() && ctl->GetParent() == this->GetParent()) {
			if (ctl->GetParent()->IsPendLayout()) {
				ctl->GetParent()->RefreshLayout();
			}
			//控件的矩形位置
			const Rect& ctlRect = ctl->GetRect();
			if (ctlRect.Y >= 0 && ctlRect.GetBottom() <= Height()) {
				return;
			}
			//出现在顶部
			int offset = this->m_offset - ctlRect.Y;
			if (ctlRect.Y > 0) {
				//出现在底部
				offset += this->m_viewLength - ctlRect.Height;
			}
			__super::ScrollTo(offset, Event::None);
		}
	}

	Rect VScrollBar::GetSliderRect() {
		Rect sliderRect;
		sliderRect.X = 0;
		sliderRect.Y = m_sliderPos;
		sliderRect.Width = Width();
		sliderRect.Height = m_sliderLength;
		if (this->Scrollable() && sliderRect.Height <= 0) {
			sliderRect.Height = 1;
		}
		return sliderRect;
	}
	void VScrollBar::GetInfo(int* viewLength, int* contentLength, int* scrollBarLength)
	{
		*viewLength = this->GetParent()->Height();
		*contentLength = this->GetParent()->GetContentSize().Height;
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
			m_mouseDown = true;
			this->m_lastPoint = point.Y;
		}
	}
	void VScrollBar::OnMouseMove(const MouseEventArgs& arg)
	{
		__super::OnMouseMove(arg);
		if (m_mouseDown) {
			auto point = arg.Location;
			int offsetY = point.Y - this->m_lastPoint;
			m_sliderPos += offsetY;
			m_lastPoint = point.Y;
			int offset = m_sliderPos * this->m_rollRate + 0.5;
			__super::ScrollTo(-offset, Event::OnMouseDrag);
		}
	}
};