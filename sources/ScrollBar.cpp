#include "ScrollBar.h"
//滚动条相关
namespace ezui {
	void ScrollBar::OnMouseUp(const MouseEventArgs& arg)
	{
		__super::OnMouseUp(arg);
		m_mouseDown = false;
	}
	void  ScrollBar::OnMouseLeave(const MouseEventArgs& arg)
	{
		__super::OnMouseLeave(arg);
		m_mouseDown = false;
	}
	ScrollBar::ScrollBar(Object* parentObject) :Control(parentObject)
	{
		Init();
	}
	void ScrollBar::Init() {
		Style.ForeColor = Color(205, 205, 205);//the bar default backgroundcolor
		SetSize({ 10,10 });
	}
	ScrollBar::~ScrollBar() {}
	bool ScrollBar::IsDraw()
	{
		if (!Scrollable()) {
			return false;
		}
		return this->IsVisible();
	}
	void ScrollBar::Reset()
	{
		this->m_sliderPos = 0;
		this->m_offset = 0;
	}
	bool ScrollBar::Scrollable()
	{
		if (this->m_overflowLength > 0) {
			return true;
		}
		return false;
	}
	void ScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (!Scrollable()) {
			return;
		}
		__super::OnBackgroundPaint(e);
	}
	void ScrollBar::OnForePaint(PaintEventArgs& args)
	{
		if (!Scrollable()) {
			return;
		}
		__super::OnForePaint(args);
		//绘制滑块
		RectF sliderRect = GetSliderRect();
		const Color& color = GetForeColor();
		if (color.GetValue() != 0) {
			args.Graphics.SetColor(color);
			args.Graphics.FillRectangle(sliderRect, GetBorderTopLeftRadius());
		}
	}

	void ScrollBar::ScrollTo(float scrollRate) {
		if (Parent == NULL) return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t offset = scrollRate * this->m_overflowLength;
		ScrollTo(-offset, Event::None);
	}

	float ScrollBar::ScrollPos()
	{
		if (this->m_overflowLength <= 0)  return 1.0f;
		return std::abs(this->m_offset) * 1.0f / this->m_overflowLength;
	}

	void ScrollBar::ScrollTo(int_t offset, const Event& type) {
		if (Parent == NULL) return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		//if (!Scrollable()) {
		//	return;
		//}
		int_t viewLength;
		int_t contentLength;
		int_t scrollBarLength;
		this->GetInfo(&viewLength, &contentLength, &scrollBarLength);
		if (offset > 0) {
			//滚动条在顶部
			this->m_offset = 0;
			this->m_sliderPos = 0;
		}
		else if (std::abs(offset) >= this->m_overflowLength) {
			//滚动条在底部
			this->m_offset = -this->m_overflowLength;
			this->m_sliderPos = scrollBarLength - this->m_sliderLength;
		}
		else {
			//正常滚动
			this->m_offset = offset;
			this->m_sliderPos = -offset / this->m_rollRate;
		}
		//调用容器的滚动函数进行偏移
		if (OffsetCallback) {
			OffsetCallback(this->m_offset);
			SyncInfo();
		}
		Parent->Invalidate();
		//Parent->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
		if (Scroll) {
			Scroll(this, (double)this->m_offset / (-this->m_overflowLength), type);
		}
	}
	void ScrollBar::SyncInfo()
	{
		if (Parent == NULL)return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t scrollBarLength;
		this->GetInfo(&this->m_viewLength, &this->m_contentLength, &scrollBarLength);
		this->m_overflowLength = this->m_contentLength - this->m_viewLength;//超出容器的内容长度
		if (m_overflowLength > 0) {
			this->m_sliderLength = (double)this->m_viewLength / this->m_contentLength * scrollBarLength + 0.5;//滑块长度
			double rollTotal = scrollBarLength - this->m_sliderLength;//当前滑块可用滑道的总距离
			this->m_rollRate = (double)(m_contentLength - this->m_viewLength) / rollTotal;//滑块每次滚动一次的对应上下文内容的比率
		}
		else {
			this->m_sliderLength = scrollBarLength;
			this->m_sliderPos = 0;
			this->m_offset = 0;
			this->m_rollRate = 0;
			this->m_overflowLength = 0;
		}
	}
	void ScrollBar::RefreshScroll() {
		SyncInfo();
		ScrollTo(this->m_offset, Event::None);
	};
	void ScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		this->m_offset += arg.ZDelta * 0.5;
		ScrollTo(this->m_offset, Event::OnMouseWheel);
	}
};