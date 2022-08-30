#include "HList.h"

namespace EzUI {
	HList::HList()
	{
		this->ScrollBar = hScrollBar = new HScrollBar;
		if (hScrollBar) {
			hScrollBar->ReSize({ Width(),10 });//���������
			hScrollBar->Parent = this;
			hScrollBar->_controlsLocationX = &_controlsLocationX;
		}
	}

	HList::~HList()
	{
	}
	void HList::SetMargin(int margin)
	{
		Margin = margin;
	}

	void HList::RefreshLayout() {
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
		ctl->Move({ _maxRight, ctl->Y() });
		_maxRight += ctl->Width();
		_maxRight += Margin;
		_controlsLocationX.insert(std::pair<Control*, int>(ctl, ctl->X()));
	}
	ControlIterator HList::RemoveControl(Control* ctl)
	{
		size_t before = _controls.size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (_controls.size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outWidth = (Margin + ctl->Width());//ɾ���ؼ��������Ŀհ�������
			_maxRight -= outWidth;//��ȥ�հ�������
			_controlsLocationX.erase(ctl);//����¼X�����mapҲҪɾ���ؼ�
			for (auto i = nextIt; i != _controls.end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
			{
				Control* it = *i;
				it->SetRect(Rect(it->X() - outWidth, it->Y(), it->Width(), it->Height()));//�����ƶ�
				_controlsLocationX[it] -= outWidth;//��¼��ԭʼ�����ƶ�
			}
			if (hScrollBar) {
				hScrollBar->SetMaxRight(_maxRight);//֪ͨ�������������߽�ֵ�Ѿ��ı�
			}
		}
		return nextIt;
	}

	void HList::Clear(bool freeControls) {
		__super::Clear(freeControls);
		_controlsLocationX.clear();
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
				it.OnEvent(Event::OnPaint, &args);
			}
			if (it.Y() >= _rect.Height) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�
				break;
			}
		}
		//�ӿؼ��������
	}
};