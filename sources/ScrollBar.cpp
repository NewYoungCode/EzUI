#include "ScrollBar.h"
//滚动条相关
namespace EzUI {
	void IScrollBar::OnMouseUp(const MouseEventArgs& arg)
	{
		__super::OnMouseUp(arg);
		_mouseDown = false;
	}
	void  IScrollBar::OnMouseLeave(const MouseEventArgs& arg)
	{
		__super::OnMouseLeave(arg);
		_mouseDown = false;
	}
	IScrollBar::IScrollBar() {
		Style.ForeColor = Color(205, 205, 205);//the bar default backgroundcolor
		SetSize({ 10,10 });
	}
	IScrollBar::~IScrollBar() {}
	bool IScrollBar::IsDraw()
	{
		if (!Scrollable()) {
			return false;
		}
		return this->IsVisible();
	}
	void IScrollBar::Reset()
	{
		this->_sliderPos = 0;
		this->_offset = 0;
	}
	bool IScrollBar::Scrollable()
	{
		if (this->_overflowLength > 0) {
			return true;
		}
		return false;
	}
	void IScrollBar::OnBackgroundPaint(PaintEventArgs& e) {
		if (!Scrollable()) {
			return;
		}
		__super::OnBackgroundPaint(e);
	}
	void IScrollBar::OnForePaint(PaintEventArgs& args)
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

	void IScrollBar::ScrollTo(float scrollRate) {
		if (Parent == NULL) return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t offset = scrollRate * this->_overflowLength;
		ScrollTo(-offset, Event::None);
	}

	float IScrollBar::ScrollPos()
	{
		if (this->_overflowLength <= 0)  return 1.0f;
		return std::abs(this->_offset) * 1.0f / this->_overflowLength;
	}

	void IScrollBar::ScrollTo(int_t offset, const Event& type) {
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
			this->_offset = 0;
			this->_sliderPos = 0;
		}
		else if (std::abs(offset) >= this->_overflowLength) {
			//滚动条在底部
			this->_offset = -this->_overflowLength;
			this->_sliderPos = scrollBarLength - this->_sliderLength;
		}
		else {
			//正常滚动
			this->_offset = offset;
			this->_sliderPos = -offset / this->_rollRate;
		}
		//调用容器的滚动函数进行偏移
		if (OffsetCallback) {
			OffsetCallback(this->_offset);
			SyncInfo();
		}
		Parent->Invalidate();
		//Parent->Refresh();//可以用Refresh,这样滚动的时候的时候显得丝滑
		if (Scroll) {
			Scroll(this, (double)this->_offset / (-this->_overflowLength), type);
		}
	}
	void IScrollBar::SyncInfo()
	{
		if (Parent == NULL)return;
		if (Parent->IsPendLayout()) {
			Parent->RefreshLayout();
		}
		int_t scrollBarLength;
		this->GetInfo(&this->_viewLength, &this->_contentLength, &scrollBarLength);
		this->_overflowLength = this->_contentLength - this->_viewLength;//超出容器的内容长度
		if (_overflowLength > 0) {
			this->_sliderLength = (double)this->_viewLength / this->_contentLength * scrollBarLength + 0.5;//滑块长度
			double rollTotal = scrollBarLength - this->_sliderLength;//当前滑块可用滑道的总距离
			this->_rollRate = (double)(_contentLength - this->_viewLength) / rollTotal;//滑块每次滚动一次的对应上下文内容的比率
		}
		else {
			this->_sliderLength = scrollBarLength;
			this->_sliderPos = 0;
			this->_offset = 0;
			this->_rollRate = 0;
			this->_overflowLength = 0;
		}
	}
	void IScrollBar::RefreshScroll() {
		SyncInfo();
		ScrollTo(this->_offset, Event::None);
	};
	void IScrollBar::OnMouseWheel(const MouseEventArgs& arg) {
		__super::OnMouseWheel(arg);
		this->_offset += arg.ZDelta * 0.5;
		ScrollTo(this->_offset, Event::OnMouseWheel);
	}
};