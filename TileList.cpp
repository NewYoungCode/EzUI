#include "TileList.h"

namespace EzUI {

	void TileList::Clear(bool freeChilds)
	{
		__super::Clear(freeChilds);
		LocationY.clear();
		_MaxBottom = 0;
		if (vScrollBar) {
			vScrollBar->SetMaxBottom(_MaxBottom);
		}
	}

	TileList::TileList()
	{
		this->ScrollBar = new VScrollBar;
		vScrollBar = (VScrollBar*)this->ScrollBar;
		if (vScrollBar) {
			vScrollBar->SetHeight(Height());
			vScrollBar->Parent = this;
			vScrollBar->_controlsLocationY = &LocationY;
		}
	}
	TileList::~TileList()
	{
	}
	bool TileList::OnSize(const Size& sz)
	{
		if (__super::OnSize(sz)) {
			ResumeLayout();
			if (vScrollBar) {
				vScrollBar->SetMaxBottom(_MaxBottom);
			}
			return true;
		}
		return false;
		
	}
	void TileList::OnChildPaint(Controls& controls, PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, _rect.Width, _rect.Height);
		//绘制子控件
		for (auto i = controls.begin(); i != controls.end(); i++)
		{
			auto& it = **i;
			if (rect.IntersectsWith(it.GetRect())) {
				VisibleControls.push_back(*i);
				it.Rending(args);
			}
			if (it.Y() >= _rect.Height) { //纵向列表控件超出则不再绘制后面的控件 优化鼠标操作的性能
				break;
			}
		}
		//子控件绘制完毕
	}
	void TileList::ResumeLayout()
	{
		if (_rect.IsEmptyArea()) return;
		LocationY.clear();
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
			it.SetLocation({ _right,_bottom });
			LocationY.insert(std::pair<Control*, int>(&it, it.Y()));
			_right += it.Width() + MarginRight;
			if (maxHeight < it.Height()) {
				maxHeight = it.Height();
			}
			_hCount++;
			itor++;
			_MaxBottom = maxHeight + it.Y();
		}

		_MaxBottom += MarginTop;

		this->PendLayout = false;
	}

};