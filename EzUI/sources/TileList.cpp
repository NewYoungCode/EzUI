#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		vScrollBar.Location.clear();
		_MaxBottom = 0;
		RefreshScroll(_MaxBottom);
	}

	void TileList::Init()
	{

		vScrollBar.SetHeight(Height());
		vScrollBar.Parent = this;
	}

	TileList::TileList()
	{
		Init();
	}
	TileList::TileList(Control* parent) :Control(parent)
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
			vScrollBar.Visible = false;
		}
		else {
			vScrollBar.Visible = true;
		}
		vScrollBar.SetMaxBottom(_maxBottom);
	}
	void TileList::OnLayout()
	{
		__super::OnLayout();

		vScrollBar.Location.clear();
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

			vScrollBar.Location.insert(std::pair<Control*, int>(&it, refY));

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