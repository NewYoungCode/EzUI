#include "VList.h"
namespace EzUI {
	void VList::Init()
	{
		vScrollBar.SetHeight(Height());//���������
		vScrollBar.Parent = this;
	}
	VList::VList()
	{
		Init();
	}
	VList::VList(Control* parent) :Control(parent)
	{
		Init();
	}
	VList::~VList()
	{
	}
	int VList::ContentLenght() {
		return _maxBottom;
	}
	void VList::OnLayout() {
		__super::OnLayout();

		_maxBottom = 0;
		for (auto& it : GetControls()) {
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
				it->SetRect(Rect{ x,_maxBottom,width,it->Height() });
			}

			_maxBottom += it->Height();
			_maxBottom += it->Margin.GetVSpace();
		}
		RefreshScroll(_maxBottom);
	}
	void VList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		_maxBottom += ctl->Margin.Top;
		int& y = (int&)ctl->Y();
		y = _maxBottom;
		_maxBottom += (ctl->Height() + ctl->Margin.Bottom);
		vScrollBar.Location.insert(std::pair<Control*, int>(ctl, ctl->Y()));
	}

	ControlIterator VList::RemoveControl(Control* ctl)
	{
		size_t before = GetControls().size();//��¼һ��ʼ�Ŀؼ�����
		ControlIterator nextIt = __super::RemoveControl(ctl);//ɾ���ؼ�
		if (GetControls().size() < before) {//����ؼ������ȿ�ʼ�� �� ɾ���ɹ�
			int outHeight = (ctl->Height() + ctl->Margin.GetVSpace());//ɾ���ؼ��������Ŀհ�������

			_maxBottom -= outHeight;//��ȥ�հ�����߶�
			vScrollBar.Location.erase(ctl);//����¼Y�����mapҲҪɾ���ؼ�
			for (ControlIterator i = nextIt; i != GetControls().end(); i++)//��ɾ������һ���ؼ���ʼ��ǰ�ƶ�X����
			{
				Control* it = *i;
				it->SetRect(Rect(it->X(), it->Y() - outHeight, it->Width(), it->Height()));//�����ƶ�
				int& locationY = vScrollBar.Location[it] -= outHeight;//��¼������ҲҪ�ƶ�
			}
			RefreshScroll(_maxBottom);//֪ͨ�������������߽�ֵ�Ѿ��ı�
		}
		return nextIt;
	}

	void VList::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		vScrollBar.Location.clear();
		_maxBottom = 0;
		RefreshScroll(_maxBottom);
	}

	void VList::OnSize(const Size& size) {
		__super::OnSize(size);
		RefreshScroll(_maxBottom);
	}

	void VList::RefreshScroll(const int& _maxBottom) {
		if (AutoHeight) {
			this->_fixedHeight = _maxBottom;
			this->_rect.Height = _maxBottom;
			vScrollBar.Visible = false;
		}
		else {
			vScrollBar.Visible = true;
		}
		vScrollBar.SetMaxBottom(_maxBottom);
	}

	ScrollBar* VList::GetScrollBar()
	{
		return &vScrollBar;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="controls"></param>
	/// <param name="args"></param>
	void VList::ChildPainting(Controls& controls, PaintEventArgs& args) {
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