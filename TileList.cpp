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
	bool TileList::OnSize(const Size& sz)
	{
		if (__super::OnSize(sz)) {
			ResumeLayout();
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_MaxBottom);
			}
			return true;
		}
		return false;

	}
	
	void TileList::ResumeLayout()
	{
		__super::ResumeLayout();
		LocationY.clear();
		_MaxBottom = 0;

		const int& maxWith = this->Width();
		int maxHeight = 0;//ÿ����ߵ��Ǹ�
		int x = 0;
		int y = 0;
		for (auto& _it : _controls) {

			Control& it = *_it;
			int _x = it.Margin.Left + it.Width();

			if (x + _x > maxWith) {
				//����
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}

			x += it.Margin.Left;//��߾�
			int& refX = ((Rect&)(it.GetRect())).X;//����x
			int& refY = ((Rect&)(it.GetRect())).Y;//����y
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
	}

};