#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		LocationY.clear();
		_MaxBottom = 0;
		RefreshScroll(_MaxBottom);
	}

	TileList::TileList()
	{
		this->vScrollBar = new VScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	TileList::~TileList()
	{
	}
	ScrollBar* TileList::GetScrollBar()
	{
		return this->vScrollBar;
	}
	void TileList::OnSize(const Size& sz)
	{
		__super::OnSize(sz);
		ResumeLayout();
		RefreshScroll(_MaxBottom);
	}

	void TileList::RefreshScroll(const int& _maxBottom) {
		if (AutoHeight) {
			this->_fixedHeight = _maxBottom;
			this->_rect.Height = _maxBottom;
			vScrollBar->Visible = false;
		}
		else {
			vScrollBar->Visible = true;
		}
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}
	void TileList::OnLayout()
	{
		__super::OnLayout();

		LocationY.clear();
		_MaxBottom = 0;

		const int& maxWith = this->Width();
		int maxHeight = 0;//每行最高的那个
		int x = 0;
		int y = 0;
		for (auto& _it : GetControls()) {

			Control& it = *_it;
			int _x = it.Margin.Left + it.Width();

			if (x + _x > maxWith) {
				//换行
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}

			x += it.Margin.Left;//左边距
			int& refX = (int&)it.X();//引用x
			int& refY = (int&)it.Y();//引用y
			refX = x;//设置X坐标
			refY = y + it.Margin.Top;//设置Y坐标+上边距

			LocationY.insert(std::pair<Control*, int>(&it, refY));

			int itemSpace = it.Height() + it.Margin.GetVSpace();//当前控件垂直占用的空间
			if (maxHeight < itemSpace) {
				maxHeight = itemSpace;
			}

			x += it.Margin.Right + it.Width();//右边距

			_MaxBottom = y + maxHeight;
		}
		RefreshScroll(_MaxBottom);
	}
	void TileList::ResumeLayout() {
		__super::ResumeLayout();
	}
};