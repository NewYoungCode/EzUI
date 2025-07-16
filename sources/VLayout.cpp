#include "VLayout.h"
namespace ezui {
	VLayout::VLayout()
	{
	}
	VLayout::VLayout(Control* parentObject):Control(parentObject)
	{
	}
	VLayout::~VLayout()
	{
	}

	void VLayout::OnLayout()
	{
		int_t contentWidth = 0;

		int_t fixedHeight = 0;
		int_t fixedTotal = 0;
		int_t count = 0;//可见控件总数
		for (auto& it : GetControls()) {
			if ((it->IsAutoWidth() || it->IsAutoHeight()) && it->IsPendLayout()) {
				it->RefreshLayout();
			}
			if (it->IsVisible() == false || (it->IsAutoHeight() && it->GetFixedHeight() <= 0))continue;
			++count;
			auto height = it->GetFixedHeight();
			if (height > 0) {
				fixedHeight += height;
				++fixedTotal;
			}
			fixedHeight += +it->Margin.GetVSpace();
		}
		int_t autoTotal = count - fixedTotal;
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
			int_t	width = it->GetFixedWidth();
			if (width == 0) {
				width = this->Width() - it->Margin.GetHSpace();
			}

			int_t x = it->X();

			//if (!IsAutoWidth()) {
			if (ContentAlign == HAlign::Left) {
				x = it->Margin.Left;
			}
			else  if (ContentAlign == HAlign::Center) {
				x = int_t((this->Width() * 1.0 - width) / 2.0f + 0.5);
			}
			else if (ContentAlign == HAlign::Right) {
				x = this->Width() - it->Margin.Right - width;
			}
			//}

			if (it->GetFixedHeight() > 0 || it->IsAutoHeight()) {
				it->SetRect({ x, (int_t)maxBottom,width, it->GetFixedHeight() });
				maxBottom += it->Height();
			}
			else {
				it->SetRect({ x ,(int_t)maxBottom,width ,(int_t)autoHeight });
				maxBottom += it->Height();
			}
			maxBottom += it->Margin.Bottom;
			//统计上下文宽高
			int_t maxRight = it->GetRect().GetRight() + it->Margin.Right;
			if (maxRight > contentWidth) {
				contentWidth = maxRight;
			}
			this->SetContentSize({ contentWidth ,(int_t)(maxBottom + 0.5) });
		}
	}
};