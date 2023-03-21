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
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
		int x = 0;
		int y = 0;
		for (auto& _it : GetControls()) {

			Control& it = *_it;
			int _x = it.Margin.Left + it.Width();

			if (x + _x > maxWith) {
				//����
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}

			x += it.Margin.Left;//��߾�
			int& refX = (int&)it.X();//����x
			int& refY = (int&)it.Y();//����y
			refX = x;//����X����
			refY = y + it.Margin.Top;//����Y����+�ϱ߾�

			LocationY.insert(std::pair<Control*, int>(&it, refY));

			int itemSpace = it.Height() + it.Margin.GetVSpace();//��ǰ�ؼ���ֱռ�õĿռ�
			if (maxHeight < itemSpace) {
				maxHeight = itemSpace;
			}

			x += it.Margin.Right + it.Width();//�ұ߾�

			_MaxBottom = y + maxHeight;
		}
		RefreshScroll(_MaxBottom);
	}
	void TileList::ResumeLayout() {
		__super::ResumeLayout();
	}
};