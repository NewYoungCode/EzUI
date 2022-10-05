#include "HList.h"

namespace EzUI {
	HList::HList()
	{
		this->ScrollBar = hScrollBar = new HScrollBar;
		if (hScrollBar) {
			hScrollBar->SetWidth(Width());//���������
			hScrollBar->Parent = this;
			hScrollBar->_controlsLocationX = &LocationX;
		}
	}

	HList::~HList()
	{
	}

	void HList::OnLayout() {
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
				it->SetRect(Rect(it->X(), y, it->Width(), height));
			}
			_maxRight += it->Width();
			_maxRight += it->Margin.GetHSpace();
		}
		if (hScrollBar) {
			hScrollBar->SetMaxRight(_maxRight);
		}
	}

	void HList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		_maxRight += ctl->Margin.Left;
		int& x = (int&)ctl->X();
		x = _maxRight;
		_maxRight += (ctl->Width()+ctl->Margin.Right);
		LocationX.insert(std::pair<Control*, int>(ctl, ctl->X()));
	}
	ControlIterator HList::RemoveControl(Control* ctl)
	{
		size_t before = GetControls().size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (GetControls().size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outWidth = ( ctl->Width() +ctl->Margin.GetHSpace());//ɾ���ؼ��������Ŀհ�������
			_maxRight -= outWidth;//��ȥ�հ�������
			LocationX.erase(ctl);//����¼X�����mapҲҪɾ���ؼ�
			for (auto i = nextIt; i != GetControls().end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
			{
				Control* it = *i;
				it->SetRect(Rect(it->X() - outWidth, it->Y(), it->Width(), it->Height()));//�����ƶ�
				LocationX[it] -= outWidth;//��¼��ԭʼ�����ƶ�
			}
			if (hScrollBar) {
				hScrollBar->SetMaxRight(_maxRight);//֪ͨ�������������߽�ֵ�Ѿ��ı�
			}
		}
		return nextIt;
	}

	void HList::Clear(bool freeControls) {
		__super::Clear(freeControls);
		LocationX.clear();
		_maxRight = 0;
		if (hScrollBar) {
			hScrollBar->SetMaxRight(_maxRight);
		}
	}

	bool HList::OnSize(const Size& size) {
		if (__super::OnSize(size)) {
			if (hScrollBar) {
				hScrollBar->SetMaxRight(_maxRight);
			}
			return true;
		}
		return false;
	}

	void HList::ChildPainting(Controls& controls, PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0,Width(), Height());
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