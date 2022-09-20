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
	void HList::SetMargin(int margin)
	{
		Margin = margin;
	}

	void HList::ResumeLayout() {
		_maxRight = 0;
		for (auto& it : _controls) {
			if (it->Width() <= 0 || it->Visible == false) continue;
			_maxRight += it->Width();
			_maxRight += Margin;
		}
		if (hScrollBar) {
			hScrollBar->SetMaxRight(_maxRight);
		}
	}

	void HList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		ctl->SetLocation({ _maxRight, ctl->Y() });
		_maxRight += ctl->Width();
		_maxRight += Margin;
		LocationX.insert(std::pair<Control*, int>(ctl, ctl->X()));
	}
	ControlIterator HList::RemoveControl(Control* ctl)
	{
		size_t before = _controls.size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (_controls.size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outWidth = (Margin + ctl->Width());//ɾ���ؼ��������Ŀհ�������
			_maxRight -= outWidth;//��ȥ�հ�������
			LocationX.erase(ctl);//����¼X�����mapҲҪɾ���ؼ�
			for (auto i = nextIt; i != _controls.end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
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

	void HList::OnSize(const Size& size) {
		__super::OnSize(size);
		if (hScrollBar) {
			hScrollBar->OnLayout(size);
			hScrollBar->SetMaxRight(_maxRight);
		}
	}

	void HList::OnChildPaint(Controls& controls, PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, _rect.Width, _rect.Height);
		//�����ӿؼ�
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.Rending(args);
			}
			if (it.Y() >= _rect.Height) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�
				break;
			}
		}
		//�ӿؼ��������
	}
};