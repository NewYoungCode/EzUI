#include "VList.h"
namespace EzUI {
	VList::VList()
	{
		this->ScrollBar = vScrollBar = new VScrollBar;
		if (vScrollBar) {
			vScrollBar->ReSize({ 10,Height() });//���������
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &_controlsLocationY;
		}
	}

	VList::~VList()
	{
	}
	void VList::SetMargin(int margin)
	{
		Margin = margin;
	}
	void VList::ResumeLayout() {
		_maxBottom = 0;
		for (auto& it : _controls) {
			if (it->Width() <= 0 || it->Visible == false) continue;
			_maxBottom += it->Height();
			_maxBottom += Margin;
		}
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}
	void VList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		ctl->Move({ ctl->X(), _maxBottom });
		_maxBottom += ctl->Height();
		_maxBottom += Margin;
		_controlsLocationY.insert(std::pair<Control*, int>(ctl, ctl->Y()));
	}

	ControlIterator VList::RemoveControl(Control* ctl)
	{
		size_t before = _controls.size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (_controls.size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outHeight = (Margin + ctl->Height());//ɾ���ؼ��������Ŀհ�������
			_maxBottom -= outHeight;//��ȥ�հ�����߶�
			_controlsLocationY.erase(ctl);//����¼Y�����mapҲҪɾ���ؼ�
			for (auto i = nextIt; i != _controls.end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
			{
				Control* it = *i;
				it->SetRect(Rect(it->X(), it->Y() - outHeight, it->Width(), it->Height()));//�����ƶ�
				int& locationY = _controlsLocationY[it] -= outHeight;//��¼������ҲҪ�ƶ�
			}
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_maxBottom);//֪ͨ�������������߽�ֵ�Ѿ��ı�
			}
		}
		return nextIt;
	}

	void VList::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		_controlsLocationY.clear();
		_maxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}

	void VList::OnSize(const Size& size) {
		__super::OnSize(size);
		if (vScrollBar) {
			vScrollBar->OnLayout(size);
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}

	void VList::OnChildPaint(Controls& controls, PaintEventArgs& args) {
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