#include "VList.h"
namespace EzUI {
	VList::VList()
	{
		this->ScrollBar = vScrollBar = new VScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());//滚动条宽度
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	VList::~VList()
	{
	}
	void VList::OnLayout() {
		__super::OnLayout();

		_maxBottom = 0;
		for (auto& it : GetControls()) {
			if (it->Visible == false)continue;

			{
				//处理margin和x坐标
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
		RefreshScroll(_maxBottom);
	}
	void VList::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		_maxBottom += ctl->Margin.Top;
		int& y = (int&)ctl->Y();
		y = _maxBottom;
		_maxBottom += (ctl->Height() + ctl->Margin.Bottom);
		LocationY.insert(std::pair<Control*, int>(ctl, ctl->Y()));
	}

	ControlIterator VList::RemoveControl(Control* ctl)
	{
		size_t before = GetControls().size();//记录一开始的控件数量
		ControlIterator nextIt = __super::RemoveControl(ctl);//删除控件
		if (GetControls().size() < before) {//如果控件数量比开始少 则 删除成功
			int outHeight = (ctl->Height() + ctl->Margin.GetVSpace());//删除控件留出来的空白区域宽度

			_maxBottom -= outHeight;//减去空白区域高度
			LocationY.erase(ctl);//将记录Y坐标的map也要删除控件
			for (ControlIterator i = nextIt; i != GetControls().end(); i++)//从删除的下一个控件开始往前移动X坐标
			{
				Control* it = *i;
				it->SetRect(Rect(it->X(), it->Y() - outHeight, it->Width(), it->Height()));//自身移动
				int& locationY = LocationY[it] -= outHeight;//记录的坐标也要移动
			}
			RefreshScroll(_maxBottom);//通知滚动条容器最大边界值已经改变
		}
		return nextIt;
	}

	void VList::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		LocationY.clear();
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
			vScrollBar->Visible = false;
		}
		else {
			vScrollBar->Visible = true;
		}
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_maxBottom);
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="controls"></param>
	/// <param name="args"></param>
	void VList::ChildPainting(Controls& controls, PaintEventArgs& args) {
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