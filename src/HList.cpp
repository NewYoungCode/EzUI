#include "HList.h"

namespace EzUI {
	void HList::Init()
	{
		this->GetScrollBar()->SetWidth(Width());//���������
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OffsetCallback = [this](int offsetValue)->void {
			if (this->GetScrollBar()->ScrollPos() >= 1) {
				NextPage();
			}
			this->Offset(offsetValue);
			};
	}
	HList::HList()
	{
		Init();
	}
	HList::~HList()
	{
	}
	void HList::OnLayout() {
		this->Offset(0);
		if (IsAutoWidth()) {
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
	}

	ScrollBar* HList::GetScrollBar()
	{
		return &hScrollBar;
	}
	void HList::Offset(int offset) {
		int _contentHeight = 0;
		int _contentWidth = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) {
				it->SetX(0);
				continue;
			}
			//����y�����margin
			{
				int height = it->GetFixedHeight();
				if (height == 0) {
					height = this->Height() - it->Margin.GetVSpace();
				}
				int y = it->Y();
				if (y == 0) {
					y = it->Margin.Top;
				}
				if (y == 0 && height < this->Height()) {
					y = int((this->Height() * 1.0 - height) / 2 + 0.5);
				}
				_contentWidth += it->Margin.Left;
				it->SetRect(Rect(_contentWidth, y, it->Width(), height));
				_contentWidth += it->Width();
				_contentWidth += it->Margin.Right;
			}
			//�������߶�
			int _height = it->Y() + it->Height() + it->Margin.Bottom;
			if (_height > _contentHeight) {
				_contentHeight = _height;
			}
		}
		this->SetContentSize({ _contentWidth - offset,_contentHeight });
	}

	void HList::OnChildPaint(PaintEventArgs& args) {
		ViewControls.clear();
		//�����ӿؼ�
		auto rect = Rect(0, 0, Width(), Height());
		for (auto& it : GetControls()) {
			if (rect.IntersectsWith(it->GetRect())) {
				ViewControls.push_back(it);
			}
			if (it->X() >= Width()) {
				//���ؼ����������ײ��������ɷ������¼� ������ȻҪ���в���
				if (it->IsAutoWidth() && it->GetLayoutState() == LayoutState::Pend) {
					it->RefreshLayout();
				}
			}
			else {
				it->DispatchEvent(args);
			}
		}
	}
};