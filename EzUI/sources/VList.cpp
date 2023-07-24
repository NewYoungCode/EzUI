#include "VList.h"
namespace EzUI {
	void VList::Init()
	{
		vScrollBar.SetHeight(Height());//滚动条宽度
		vScrollBar.Parent = this;
		vScrollBar.OWner = this;
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
	void VList::OnLayout() {
		_contentWidth = 0;
		_contentWidth = MoveScroll(0);
		RefreshScroll(_contentWidth);
		if (AutoHeight && this->Height() != _contentWidth) {
			this->SetFixedHeight(_contentWidth);
			if (Parent) {
				Parent->Invalidate();
			}
		}
	}

	void VList::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		if (attrName == "height" && attrValue == "auto") {
			this->AutoHeight = true;
			return;
		}
		__super::SetAttribute(attrName, attrValue);
	}

	void VList::RefreshScroll(const int& _maxBottom) {
		if (AutoHeight) {
			vScrollBar.SetVisible(false);
		}
		else if (vScrollBar.IsVisible() == true) {
			vScrollBar.SetVisible(true);
		}
		vScrollBar.SetMaxBottom(_maxBottom);
	}

	ScrollBar* VList::GetScrollBar()
	{
		return &vScrollBar;
	}

	int VList::MoveScroll(int offset)
	{
		_contentWidth = 0;
		int	_maxBottom = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false)continue;
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
				it->SetRect(Rect{ x,_maxBottom,width,it->Height() });
			}
			_maxBottom += it->Height();
			_maxBottom += it->Margin.GetVSpace();
			//计算最大宽度
			int _width = it->X() + it->Width();
			if (_width > _contentWidth) {
				_contentWidth = _width;
			}
		}
		return _maxBottom;
	}

	bool VList::IsAutoHeight()
	{
		return this->AutoHeight;
	}

	void VList::SetAutoHeight(bool flag)
	{
		if (flag != this->AutoHeight && Parent) {
			Parent->TryPendLayout();
		}
		this->AutoHeight = flag;
	}

	void VList::OnChildPaint(PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, Width(), Height());
		//绘制子控件
		for (auto i = GetControls().begin(); i != GetControls().end(); i++)
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