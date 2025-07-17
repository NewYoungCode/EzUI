#include "HListView.h"

namespace ezui {
	void HListView::Init()
	{
		this->GetScrollBar()->SetWidth(Width());//滚动条宽度
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [this](int_t offsetValue)->void {
			if (this->GetScrollBar()->ScrollPos() >= 1) {
				NextPage();
			}
			this->Offset(offsetValue);
			};
	}
	HListView::HListView(Object* parentObject):PagedListView(parentObject)
	{
		Init();
	}
	HListView::~HListView()
	{
	}
	void HListView::OnLayout() {
		this->Offset(0);
		if (IsAutoWidth()) {
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
	}

	ScrollBar* HListView::GetScrollBar()
	{
		return &m_hScrollBar;
	}
	void HListView::Offset(int_t offset) {
		int_t _contentHeight = 0;
		int_t _contentWidth = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) {
				it->SetX(0);
				continue;
			}
			//处理y坐标和margin
			{
				int_t height = it->GetFixedHeight();
				if (height == 0) {
					height = this->Height() - it->Margin.GetVSpace();
				}
				int_t y = it->Y();
				if (y == 0) {
					y = it->Margin.Top;
				}
				if (y == 0 && height < this->Height()) {
					y = int_t((this->Height() * 1.0 - height) / 2 + 0.5);
				}
				_contentWidth += it->Margin.Left;
				it->SetRect(Rect(_contentWidth, y, it->Width(), height));
				_contentWidth += it->Width();
				_contentWidth += it->Margin.Right;
			}
			//计算最大高度
			int_t _height = it->Y() + it->Height() + it->Margin.Bottom;
			if (_height > _contentHeight) {
				_contentHeight = _height;
			}
		}
		this->SetContentSize({ _contentWidth - offset,_contentHeight });
	}

	void HListView::OnChildPaint(PaintEventArgs& args) {
		ViewControls.clear();
		//绘制子控件
		auto rect = Rect(0, 0, Width(), Height());
		for (auto& it : GetControls()) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			if (it->X() >= Width()) {
				//当控件超出容器底部将不再派发绘制事件 但是仍然要进行布局
				if (it->IsAutoWidth() && it->GetLayoutState() == LayoutState::Pend) {
					it->RefreshLayout();
				}
			}
			else {
				it->SendNotify(args);
			}
		}
	}
};