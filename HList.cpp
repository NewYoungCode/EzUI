#include "HList.h"

namespace EzUI {
	HList::HList()
	{
		this->ScrollBar = hScrollBar = new HScrollBar;
		if (hScrollBar) {
			hScrollBar->ReSize({ Width(),10 });//滚动条宽度
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
		size_t before = _controls.size();//记录一开始的控件数量
		ControlIterator nextIt = __super::RemoveControl(ctl);//删除控件
		if (_controls.size() < before) {//如果控件数量比开始少 则 删除成功
			int outWidth = (Margin + ctl->Width());//删除控件留出来的空白区域宽度
			_maxRight -= outWidth;//减去空白区域宽度
			_controlsLocationX.erase(ctl);//将记录X坐标的map也要删除控件
			for (auto i = nextIt; i != _controls.end(); i++)//从删除的下一个控件开始往前移动X坐标
			{
				Control* it = *i;
				it->SetRect(Rect(it->X() - outWidth, it->Y(), it->Width(), it->Height()));//自身移动
				_controlsLocationX[it] -= outWidth;//记录的原始坐标移动
			}
			if (hScrollBar) {
				hScrollBar->SetMaxRight(_maxRight);//通知滚动条容器最大边界值已经改变
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