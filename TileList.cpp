#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		LocationY.clear();
		_MaxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}

	TileList::TileList()
	{
		this->ScrollBar = new VScrollBar;
		vScrollBar = (VScrollBar*)this->ScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	TileList::~TileList()
	{
	}
	void TileList::OnSize(const Size& sz)
	{
		__super::OnSize(sz);
		ResumeLayout();
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}
	
	void TileList::OnLayout()
	{
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
	}

};