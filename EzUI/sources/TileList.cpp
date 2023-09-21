#include "TileList.h"

namespace EzUI {

	void TileList::Init()
	{
		this->GetScrollBar()->SetHeight(Height());
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [this](int offsetValue)->void {
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
		return &vScrollBar;
	}

	void TileList::OnLayout()
	{
		this->SetContentHeight(this->Offset(0));
		if (AutoHeight) {
			this->SetFixedHeight(this->GetContentSize().Height);
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
	}
	int TileList::Offset(int offset) {
		int contentWidth = 0;
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
			if (_width > contentWidth) {
				contentWidth = _width;
			}
		}
		this->SetContentWidth(contentWidth);
		return _contentHeight;
	}
};