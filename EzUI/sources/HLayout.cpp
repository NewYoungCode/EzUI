#include "HLayout.h"
namespace EzUI {
	void HLayout::OnLayout()
	{
		int fixedWidth = 0;
		int fixedTotal = 0;
		int count = 0;//可见控件总数
		for (auto it : GetControls()) {
			if (it->Visible == false) continue;
			count++;
			auto width = it->GetFixedWidth();
			if (width > 0) {
				fixedWidth += width;
				fixedTotal++;
			}
			fixedWidth += +it->Margin.GetHSpace();
		}
		int autoTotal = count - fixedTotal;
		if (autoTotal == 0) {
			//return;
		}
		double otherWidth = Width() * 1.0 - fixedWidth;
		double autoWidth = otherWidth / autoTotal;
		double maxRight = 0;
		//排序
		for (auto& it : GetControls()) {
			if (it->Visible == false) continue;

			maxRight += it->Margin.Left;
			int height = it->GetFixedHeight();
			if (height == 0) {
				height = this->Height() - it->Margin.GetVSpace();
			}

			int y = it->Y();
			if (y == 0) {
				y = it->Margin.Top;
			}
			////首次布局中当位置未被指定的时候进行一次居中
			//if (y == 0 && height <this->Height() ) {
			//	y = int((this->Height() * 1.0 - height) / 2 + 0.5);
			//}
			if (it->GetFixedWidth() > 0) {
				it->SetRect({ (int)maxRight ,y,it->GetFixedWidth() ,height });
				maxRight += it->Width();
			}
			else {
				it->SetRect({ (int)maxRight ,y,(int)autoWidth,height });
				maxRight += autoWidth;
			}

			maxRight += it->Margin.Right;
		}
	}
	HLayout::HLayout()
	{
	}
	HLayout::HLayout(Control* parent):Control(parent)
	{
	}
	HLayout::~HLayout()
	{
	}
	void HLayout::OnSize(const Size& size) {
		__super::OnSize(size);
		ResumeLayout();
	}
};