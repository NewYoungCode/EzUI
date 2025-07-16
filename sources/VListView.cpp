#include "VListView.h"
namespace ezui {
	void VListView::Init()
	{
		this->GetScrollBar()->SetHeight(Height());//滚动条宽度
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [this](int_t offsetValue)->void {
			if (this->GetScrollBar()->ScrollPos() >= 1) {
				NextPage();
			}
			this->Offset(offsetValue);
			};
	}
	VListView::VListView()
	{
		Init();
	}
	VListView::VListView(Control* parentObject):PagedListView(parentObject)
	{
		Init();
	}
	VListView::~VListView()
	{
	}
	void VListView::OnLayout() {
		this->Offset(0);
		if (IsAutoHeight()) {
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
		this->EndLayout();
	}

	IScrollBar* VListView::GetScrollBar()
	{
		return &m_vScrollBar;
	}

	void VListView::Offset(int_t offset)
	{
		int_t contentWidth = 0;
		int_t	_maxBottom = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) {
				it->SetY(0);
				continue;
			}
			{
				//处理margin和x坐标
				int_t	width = it->GetFixedWidth();
				if (width == 0) {
					width = this->Width() - it->Margin.GetHSpace();
				}
				int_t x = it->X();
				if (x == 0) {
					x = it->Margin.Left;
				}
				if (x == 0 && width < this->Width()) {
					x = int_t((this->Width() * 1.0 - width) / 2 + 0.5);
				}
				_maxBottom += it->Margin.Top;
				it->SetRect(Rect{ x,_maxBottom,width,it->Height() });
				_maxBottom += it->Height();
				_maxBottom += it->Margin.Bottom;
			}
			//计算最大宽度
			int_t _width = it->X() + it->Width() + it->Margin.Right;
			if (_width > contentWidth) {
				contentWidth = _width;
			}
		}
		this->SetContentSize({ contentWidth, _maxBottom - offset });
	}

	void VListView::OnChildPaint(PaintEventArgs& args) {
		ViewControls.clear();
		//绘制子控件
		auto rect = Rect(0, 0, Width(), Height());
		for (auto& it : GetControls()) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			if (it->Y() >= Height()) {
				//当控件超出容器底部将不再派发绘制事件 但是仍然要进行布局
				if (it->IsAutoHeight() && it->GetLayoutState() == LayoutState::Pend) {
					it->RefreshLayout();
				}
			}
			else {
				it->SendNotify(args);
			}
		}
	}
};