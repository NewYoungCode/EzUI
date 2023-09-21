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
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
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
			if (_width > contentWidth) {
				contentWidth = _width;
			}
		}
		this->SetContentWidth(contentWidth);
		return _contentHeight;
	}
};