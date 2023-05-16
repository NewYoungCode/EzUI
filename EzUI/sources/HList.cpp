#include "HList.h"

namespace EzUI {
	void HList::Init()
	{
		hScrollBar.SetWidth(Width());//���������
		hScrollBar.Parent = this;
	}
	HList::HList()
	{
		Init();
	}
	HList::HList(Control* parent) :Control(parent)
	{
		Init();
	}
	HList::~HList()
	{
	}

	int HList::ContentLenght() {

		return _maxRight;
	}

	void HList::OnLayout() {
		__super::OnLayout();
		hScrollBar.Location.clear();
		_maxRight = 0;
		for (auto& it : GetControls()) {
			if (it->Visible == false) continue;
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
				it->SetRect(Rect(_maxRight, y, it->Width(), height));
			}
			hScrollBar.Location.insert(std::pair<Control*, int>(it, it->X()));
			_maxRight += it->Width();
			_maxRight += it->Margin.GetHSpace();
		}
		RefreshScroll(_maxRight);
	}
	void HList::RefreshScroll(const int& _maxRight) {
		if (AutoWidth) {
			this->_fixedWidth = _maxRight;
			this->_rect.Width = _maxRight;
			hScrollBar.Visible = false;
		}
		else {
			hScrollBar.Visible = true;
		}
		hScrollBar.SetMaxRight(_maxRight);
	}
	ScrollBar* HList::GetScrollBar()
	{
		return &hScrollBar;
	}
	void HList::ChildPainting(Controls& controls, PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, Width(), Height());
		//�����ӿؼ�
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.Rending(args);
			}
			if (it.Y() >= Height()) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�
				break;
			}
		}
		//�ӿؼ��������
	}
};