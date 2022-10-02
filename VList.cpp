#include "VList.h"
namespace EzUI {
	VList::VList()
	{
		this->ScrollBar = vScrollBar = new VScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());//���������
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	VList::~VList()
	{
	}

	void VList::ResumeLayout() {
		__super::ResumeLayout();

		_maxBottom = 0;
		for (auto& it : _controls) {
			if (it->Visible == false)continue;

			{
				//����margin��x����
				int	width = it->GetFixedWidth();
				if (width == 0) {
					width = this->Width() - it->Margin.GetHSpace();
				}
				int x = it->X();
				if (x == 0) {
					x = it->Margin.Left;
				}
				if (x == 0 && width < this->Width()) {
					x = int((this->Width() * 1.0 - width) / 2 + 0.5);
				}
				it->SetRect(Rect{ x,it->Y(),width,it->Height() });
			}

			_maxBottom += it->Height();
			_maxBottom += it->Margin.GetVSpace();
		}
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}
	void VList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		_maxBottom += ctl->Margin.Top;
		int& y = ((Rect&)ctl->GetRect()).Y;
		y = _maxBottom;
		_maxBottom += (ctl->Height() + ctl->Margin.Bottom);
		LocationY.insert(std::pair<Control*, int>(ctl, ctl->Y()));
	}

	ControlIterator VList::RemoveControl(Control* ctl)
	{
		size_t before = _controls.size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (_controls.size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outHeight = (ctl->Height() + ctl->Margin.GetVSpace());//ɾ���ؼ��������Ŀհ�������

			_maxBottom -= outHeight;//��ȥ�հ�����߶�
			LocationY.erase(ctl);//����¼Y�����mapҲҪɾ���ؼ�
			for (auto i = nextIt; i != _controls.end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
			{
				Control* it = *i;
				it->SetRect(Rect(it->X(), it->Y() - outHeight, it->Width(), it->Height()));//�����ƶ�
				int& locationY = LocationY[it] -= outHeight;//��¼������ҲҪ�ƶ�
			}
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_maxBottom);//֪ͨ�������������߽�ֵ�Ѿ��ı�
			}
		}
		return nextIt;
	}

	void VList::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		LocationY.clear();
		_maxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}

	bool VList::OnSize(const Size& size) {
		if (__super::OnSize(size)) {
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_maxBottom);
			}
			return true;
		}
		return false;
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
				it.Rending(args);
			}
			if (it.Y() >= _rect.Height) { //�����б�ؼ��������ٻ��ƺ���Ŀؼ� �Ż�
				break;
			}
		}
		//�ӿؼ��������
	}
};