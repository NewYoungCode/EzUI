#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
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

	void TileList::RefreshScroll(const int& _maxBottom) {
		if (AutoHeight) {
			this->_fixedHeight = _maxBottom;
			this->_rect.Height = _maxBottom;
			vScrollBar.Visible = false;
		}
		else if (vScrollBar.Visible == true) {
			vScrollBar.Visible = true;
		}
		vScrollBar.SetMaxBottom(_maxBottom);
	}
	void TileList::OnLayout()
	{
		__super::OnLayout();
		_MaxBottom = 0;
		_MaxBottom = MoveScroll(0);
		RefreshScroll(_MaxBottom);
	}
	int TileList::MoveScroll(int offset) {
		int _MaxBottom = 0;
		const int& maxWith = this->Width();
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
		int x = 0;
		int y = offset;
		for (auto& _it : GetControls()) {
			if (_it->Visible == false)continue;
			Control& it = *_it;
			int _x = it.Margin.Left + it.Width();

			if (x + _x > maxWith) {
				//����
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}

			x += it.Margin.Left;//��߾�
			int newX = x;//����X����
			int newY = y + it.Margin.Top;//����Y����+�ϱ߾�
			it.SetRect(Rect(newX, newY, it.Width(), it.Height()));
			int itemSpace = it.Height() + it.Margin.GetVSpace();//��ǰ�ؼ���ֱռ�õĿռ�
			if (maxHeight < itemSpace) {
				maxHeight = itemSpace;
			}
			x += it.Margin.Right + it.Width();//�ұ߾�
			_MaxBottom = y + maxHeight;
		}
		return _MaxBottom;
	}
};