#include "VLayout.h"
namespace EzUI {
	VLayout::VLayout()
	{
	}
	VLayout::~VLayout()
	{
	}

	void VLayout::OnLayout()
	{

		int fixedHeight = 0;
		int fixedTotal = 0;
		int count = 0;//可见控件总数
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false || (it->IsAutoHeight() && it->GetFixedHeight() <= 0))continue;
			count++;
			auto height = it->GetFixedHeight();
			if (height > 0) {
				fixedHeight += height;
				fixedTotal++;
			}
			fixedHeight += +it->Margin.GetVSpace();
		}
		int autoTotal = count - fixedTotal;
		if (autoTotal == 0) {
			//return;
		}
		double otherHeight = Height() * 1.0 - fixedHeight;
		double autoHeight = otherHeight / autoTotal;
		double maxBottom = 0;
		//排序
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false)continue;

			maxBottom += it->Margin.Top;
			int	width = it->GetFixedWidth();
			if (width == 0) {
				width = this->Width() - it->Margin.GetHSpace();
			}
			int x = it->X();
			if (x == 0) {
				x = it->Margin.Left;
			}
			//首次布局中当位置未被指定的时候进行一次居中
			//if (x == 0 && width < this->Width()) {
			x = int((this->Width() * 1.0 - width) / 2 + 0.5);
			//}
			if (it->GetFixedHeight() > 0 || it->IsAutoHeight()) {
				it->SetRect({ x, (int)maxBottom,width, it->GetFixedHeight() });
				maxBottom += it->Height();
			}
			else {
				it->SetRect({ x ,(int)maxBottom,width ,(int)autoHeight });
				maxBottom += it->Height();
			}
			maxBottom += it->Margin.Bottom;
		}
	}
};