#include "TileList.h"

namespace EzUI {

	void TileList::Init()
	{
		this->GetScrollBar()->SetHeight(Height());
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [this](int offsetValue)->void {
			if (this->GetScrollBar()->ScrollPos() >= 1) {
				NextPage();
			}
			this->Offset(offsetValue);
			};
	}

	TileList::TileList()
	{
		Init();
	}
	TileList::~TileList()
	{
	}
	ScrollBar* TileList::GetScrollBar()
	{
		return &_vScrollBar;
	}
	void TileList::OnChildPaint(PaintEventArgs& args)
	{
		ViewControls.clear();
		//绘制子控件
		auto rect = Rect(0, 0, Width(), Height());
		for (auto& it : GetControls()) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			if (it->Y() >= Height()) {
				//当控件超出容器底部将不再派发绘制事件 但是仍然要进行布局
				if ((it->IsAutoWidth() || it->IsAutoHeight()) && it->GetLayoutState() == LayoutState::Pend) {
					it->RefreshLayout();
				}
			}
			else {
				it->DispatchEvent(args);
			}
		}
	}
	void TileList::OnLayout()
	{
		this->Offset(0);
		if (_autoHeight) {
			this->SetFixedHeight(this->GetContentSize().Height);
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
	}
	void TileList::Offset(int offset) {
		int _contentWidth = 0;
		int _contentHeight = 0;
		const int& maxWith = this->Width();
		int maxHeight = 0;//每行最高的那个
		int x = 0;
		int y = offset;
		for (auto& _it : GetControls()) {
			if (_it->IsVisible() == false) {
				_it->SetLocation(Point{ 0,0 });
				continue;
			}
			Control& it = *_it;
			int _x = it.Margin.Left + it.Width();

			if (x + _x > maxWith) {
				//换行
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}

			x += it.Margin.Left;//左边距
			int newX = x;//设置X坐标
			int newY = y + it.Margin.Top;//设置Y坐标+上边距
			it.SetRect(Rect(newX, newY, it.Width(), it.Height()));
			int itemSpace = it.Height() + it.Margin.GetVSpace();//当前控件垂直占用的空间
			if (maxHeight < itemSpace) {
				maxHeight = itemSpace;
			}
			x += it.Margin.Right + it.Width();//右边距
			_contentHeight = y + maxHeight;
			//计算最大宽度
			int _width = it.X() + it.Width();
			if (_width > _contentWidth) {
				_contentWidth = _width;
			}
		}
		this->SetContentSize({ _contentWidth,_contentHeight - offset });
	}
};