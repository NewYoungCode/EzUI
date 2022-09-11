#include "VList.h"
namespace EzUI {
	VList::VList()
	{
		this->ScrollBar = vScrollBar = new VScrollBar;
		if (vScrollBar) {
			vScrollBar->ReSize({ 10,Height() });//滚动条宽度
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
		size_t before = _controls.size();//记录一开始的控件数量
		ControlIterator nextIt = __super::RemoveControl(ctl);//删除控件
		if (_controls.size() < before) {//如果控件数量比开始少 则 删除成功
			int outHeight = (Margin + ctl->Height());//删除控件留出来的空白区域宽度
			_maxBottom -= outHeight;//减去空白区域高度
			_controlsLocationY.erase(ctl);//将记录Y坐标的map也要删除控件
			for (auto i = nextIt; i != _controls.end(); i++)//从删除的下一个控件开始往前移动X坐标
			{
				Control* it = *i;
				it->SetRect(Rect(it->X(), it->Y() - outHeight, it->Width(), it->Height()));//自身移动
				int& locationY = _controlsLocationY[it] -= outHeight;//记录的坐标也要移动
			}
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_maxBottom);//通知滚动条容器最大边界值已经改变
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
		//绘制子控件
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.OnEvent(Event::OnPaint, &args);
			}
			if (it.Y() >= _rect.Height) { //纵向列表控件超出则不再绘制后面的控件 优化
				break;
			}
		}
		//子控件绘制完毕
	}
};