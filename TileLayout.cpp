#include "TileLayout.h"

namespace EzUI {

	

	void TileLayout::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		_controlsLocationY.clear();
		_MaxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}

	TileLayout::TileLayout()
	{
		this->ScrollBar = new VScrollBar;
		vScrollBar = (VScrollBar*)this->ScrollBar;
		if (vScrollBar) {
			vScrollBar->SetSize({ 10,Height() });//滚动条宽度
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &_controlsLocationY;
		}
	}
	TileLayout::~TileLayout()
	{
	}
	void TileLayout::OnSize(const Size& sz)
	{
		ResumeLayout();
		if (vScrollBar) {
			vScrollBar->OnLayout(sz);
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}
	void TileLayout::OnChildPaint(Controls& controls, PaintEventArgs& args) {
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
			if (it.Y() >= _rect.Height) { //纵向列表控件超出则不再绘制后面的控件 优化鼠标操作的性能
				break;
			}
		}
		//子控件绘制完毕
	}
	void TileLayout::ResumeLayout()
	{
		if (_rect.IsEmptyArea()) return;
		_controlsLocationY.clear();
		int _right = MarginRight;//右边距
		int _bottom = MarginTop;//上边距
		int maxHeight = 0;//每行最高的那个
		int _lineCount = 0;//一共有几行
		int _hCount = 0;//横着一行放了几个
		_MaxBottom = 0;//滚动条参数
		ControlIterator itor = _controls.begin();
		for (; itor != _controls.end(); )
		{
			Control& it = **itor;
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
			it.OnLayout({ Width(),Height() }, false);//触发原始布局特性
			it.SetLocation({ _right,_bottom });
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

};