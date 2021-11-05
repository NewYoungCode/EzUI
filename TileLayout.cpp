#include "TileLayout.h"

void TileLayout::Sort()
{
	if (_rect.IsEmptyArea()) return;
	_controlsLocationY.clear();
	int _right = MarginRight;//右边距
	int _bottom = MarginTop;//上边距
	int maxHeight = 0;//每行最高的那个
	int _lineCount = 0;//一共有几行
	int _hCount = 0;//横着一行放了几个
	_MaxBottom = 0;//滚动条参数
	std::list<Control*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); )
	{
		Control &it = **itor;
		if (_right + it.Width() > Width()) {//换行
			if (_hCount > 0) {
				_hCount = 0;
				continue;
			}
			_right = MarginRight;
			_bottom += maxHeight + MarginTop;
			maxHeight = 0;
			_lineCount++;
		}
		it.OnLayout({ Width(),Height() },false);//触发原始布局特性
		it.Move({ _right,_bottom });
		_controlsLocationY.insert(std::pair<Control*, int>(&it, it.Y()));
		_right += it.Width() + MarginRight;
		if (maxHeight < it.Height()) {
			maxHeight = it.Height();
		}
		_hCount++;
		itor++;
		_MaxBottom = maxHeight + it.Y();
	}

	_MaxBottom += MarginTop;
}

TileLayout::TileLayout()
{
	this->ScrollBar = new VScrollBar;
	verticalScrollBar = (VScrollBar*)this->ScrollBar;
	if (verticalScrollBar) {
		verticalScrollBar->ReSize({ 10,Height() });//滚动条宽度
		verticalScrollBar->Parent = this;
		verticalScrollBar->_controlsLocationY = &_controlsLocationY;
	}
}

void TileLayout::AddControl(Control * ctl)
{
	__super::AddControl(ctl);
	Sort();
	if (verticalScrollBar) {
		verticalScrollBar->SetMaxBottom(_MaxBottom);
	}
}

void TileLayout::OnSize(const Size & sz)
{
	Sort();
	if (verticalScrollBar) {
		verticalScrollBar->OnLayout(sz);
		verticalScrollBar->SetMaxBottom(_MaxBottom);
	}
}

void TileLayout::OnChildPaint(Controls &controls, PaintEventArgs &args) {
	VisibleControls.clear();
	auto rect = Rect(0, 0, _rect.Width, _rect.Height);
	//绘制子控件
	for (auto i = controls.begin(); i != controls.end(); i++)
	{
		auto &it = **i;
		if (rect.IntersectsWith(it.GetRect())) {
			VisibleControls.push_back(*i);
			it.OnEvent(Event::OnPaint, &args);
		}
		if (it.Y() >= _rect.Height) { //纵向列表控件超出则不再绘制后面的控件 优化鼠标操作的性能
			break;
		}
	}
	//子控件绘制完毕
}

void TileLayout::RefreshLayout()
{
	Sort();
	Refresh();
}
