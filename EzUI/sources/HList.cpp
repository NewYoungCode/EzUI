#include "HList.h"

namespace EzUI {
	void HList::Init()
	{
		this->GetScrollBar()->SetWidth(Width());//滚动条宽度
		this->GetScrollBar()->Parent = this;
		this->GetScrollBar()->OWner = this;
		this->GetScrollBar()->OffsetCallback = [this](int offsetValue)->void {
			this->Offset(offsetValue);
		};
	}
	HList::HList()
	{
		Init();
	}
	HList::~HList()
	{
	}
	void HList::OnLayout() {
		_contentWidth = 0;
		_contentWidth = Offset(0);
		if (AutoWidth) {
			this->GetScrollBar()->SetVisible(false);
		}
		else if (this->GetScrollBar()->IsVisible() == true) {
			this->GetScrollBar()->SetVisible(true);
		}
		if (AutoWidth && this->Width() != _contentWidth) {
			this->SetFixedWidth(_contentWidth);
			if (Parent) {
				Parent->Invalidate();
			}
		}
		this->GetScrollBar()->RefreshScroll();

	}

	void HList::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		if (attrName == "width" && attrValue == "auto") {
			this->AutoWidth = true;
			return;
		}
		__super::SetAttribute(attrName, attrValue);
	}


	ScrollBar* HList::GetScrollBar()
	{
		return &hScrollBar;
	}
	int HList::Offset(int offset) {
		_contentHeight = 0;
		int _contentWidth = offset;
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) continue;
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
				it->SetRect(Rect(_contentWidth, y, it->Width(), height));
			}
			_contentWidth += it->Width();
			_contentWidth += it->Margin.GetHSpace();
			//计算最大高度
			int _height = it->Y() + it->Height();
			if (_height > _contentHeight) {
				_contentHeight = _height;
			}
		}
		return _contentWidth;
	}
	bool HList::IsAutoWidth()
	{
		return this->AutoWidth;
	}
	void HList::SetAutoWidth(bool flag)
	{
		if (flag != this->AutoWidth && Parent) {
			Parent->TryPendLayout();
		}
		this->AutoWidth = flag;
	}
	void HList::OnChildPaint(PaintEventArgs& args) {
		VisibleControls.clear();
		auto rect = Rect(0, 0, Width(), Height());
		//绘制子控件
		for (auto i = GetControls().begin(); i != GetControls().end(); i++)
		{
			auto& it = **i;
			VisibleControls.push_back(*i);
			it.DispatchEvent(args);
			if (it.Y() >= Height()) { //纵向列表控件超出则不再绘制后面的控件 优化
				break;
			}
		}
		//子控件绘制完毕
	}
};