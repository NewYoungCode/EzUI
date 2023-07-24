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
		vScrollBar.OWner = this;
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

	void TileList::RefreshScroll(const int& _contentHeight) {
		if (AutoHeight) {
			this->SetFixedHeight(_contentHeight);
			vScrollBar.SetVisible(false);
		}
		else if (vScrollBar.IsVisible() == true) {
			vScrollBar.SetVisible(true);
		}
		vScrollBar.SetMaxBottom(_contentHeight);
	}
	void TileList::OnLayout()
	{
		_contentHeight = 0;
		_contentHeight = MoveScroll(0);
		RefreshScroll(_contentHeight);
	}
	int TileList::MoveScroll(int offset) {
		_contentWidth = 0;
		int _contentHeight = 0;
		const int& maxWith = this->Width();
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
		int x = 0;
		int y = offset;
		for (auto& _it : GetControls()) {
			if (_it->IsVisible() == false)continue;
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
			_contentHeight = y + maxHeight;
			//���������
			int _width = it.X() + it.Width();
			if (_width > _contentWidth) {
				_contentWidth = _width;
			}
		}
		return _contentHeight;
	}
};