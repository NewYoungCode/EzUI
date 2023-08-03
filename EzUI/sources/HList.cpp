#include "HList.h"

namespace EzUI {
	void HList::Init()
	{
		this->GetScrollBar()->SetWidth(Width());//���������
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OWner = this;
		this->GetScrollBar()->OffsetCallback = [this](int offsetValue)->void {
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
		this->SetContentWidth(this->Offset(0));
		if (IsAutoWidth()) {
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		this->GetScrollBar()->RefreshScroll();
	}

	void HList::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		if (attrName == "width" && attrValue == "auto") {
			this->SetAutoWidth(true);
			return;
		}
		__super::SetAttribute(attrName, attrValue);
	}


	ScrollBar* HList::GetScrollBar()
	{
		return &hScrollBar;
	}
	int HList::Offset(int offset) {
		int contentHeight = 0;
		int _contentWidth = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) continue;
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
				it->SetRect(Rect(_contentWidth, y, it->Width(), height));
			}
			_contentWidth += it->Width();
			_contentWidth += it->Margin.GetHSpace();
			//�������߶�
			int _height = it->Y() + it->Height() + it->Margin.Bottom;
			if (_height > contentHeight) {
				contentHeight = _height;
			}
		}
		this->SetContentHeight(contentHeight);
		return _contentWidth;
	}

	void HList::OnChildPaint(PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, Width(), Height());
		//�����ӿؼ�
		for (auto i = GetControls().begin(); i != GetControls().end(); i++)
		{
			auto& it = **i;
			VisibleControls.push_back(*i);
			it.DispatchEvent(args);
			if (it.Y() >= Height()) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�
				break;
			}
		}
		//�ӿؼ��������
	}
};