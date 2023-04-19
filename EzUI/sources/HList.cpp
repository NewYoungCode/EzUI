#include "HList.h"

namespace EzUI {
	void HList::Init()
	{
		hScrollBar.SetWidth(Width());//滚动条宽度
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

		_maxRight = 0;
		for (auto& it : GetControls()) {
			if (it->Visible == false) continue;
			//处理y坐标和margin
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
		RefreshScroll(_maxRight);
	}

	void HList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		_maxRight += ctl->Margin.Left;
		int& x = (int&)ctl->X();
		x = _maxRight;
		_maxRight += (ctl->Width() + ctl->Margin.Right);
		hScrollBar.Location.insert(std::pair<Control*, int>(ctl, ctl->X()));
	}
	ControlIterator HList::RemoveControl(Control* ctl)
	{
		size_t before = GetControls().size();//记录一开始的控件数量
		ControlIterator nextIt = __super::RemoveControl(ctl);//删除控件
		if (GetControls().size() < before) {//如果控件数量比开始少 则 删除成功
			int outWidth = (ctl->Width() + ctl->Margin.GetHSpace());//删除控件留出来的空白区域宽度
			_maxRight -= outWidth;//减去空白区域宽度
			hScrollBar.Location.erase(ctl);//将记录X坐标的map也要删除控件
			for (auto i = nextIt; i != GetControls().end(); i++)//从删除的下一个控件开始往前移动X坐标
			{
				Control* it = *i;
				it->SetRect(Rect(it->X() - outWidth, it->Y(), it->Width(), it->Height()));//自身移动
				hScrollBar.Location[it] -= outWidth;//记录的原始坐标移动
			}
			RefreshScroll(_maxRight);//通知滚动条容器最大边界值已经改变
		}
		return nextIt;
	}

	void HList::Clear(bool freeControls) {
		__super::Clear(freeControls);
		hScrollBar.Location.clear();
		_maxRight = 0;
		RefreshScroll(_maxRight);
	}

	void HList::OnSize(const Size& size) {
		__super::OnSize(size);
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
		//绘制子控件
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.Rending(args);
			}
			if (it.Y() >= Height()) { //纵向列表控件超出则不再绘制后面的控件 优化
				break;
			}
		}
		//子控件绘制完毕
	}
};