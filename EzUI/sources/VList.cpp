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
		vScrollBar.Location.clear();
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
			vScrollBar.Location.insert(std::pair<Control*, int>(it, it->Y()));
			_maxBottom += it->Height();
			_maxBottom += it->Margin.GetVSpace();
		}
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