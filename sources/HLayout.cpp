#include "HLayout.h"
namespace ezui {
	void HLayout::OnLayout()
	{
		int_t contentHeight = 0;
		int_t fixedWidth = 0;
		int_t fixedTotal = 0;
		int_t count = 0;//可见控件总数
		for (auto& it : GetControls()) {
			if ((it->IsAutoWidth() || it->IsAutoHeight()) && it->IsPendLayout()) {
				it->RefreshLayout();
			}
			if (it->IsVisible() == false || (it->IsAutoWidth() && it->GetFixedWidth() <= 0)) continue;
			++count;
			auto width = it->GetFixedWidth();
			if (width > 0) {
				fixedWidth += width;
				++fixedTotal;
			}
			fixedWidth += +it->Margin.GetHSpace();
		}
		int_t autoTotal = count - fixedTotal;
		if (autoTotal == 0) {
			//return;
		}
		double otherWidth = Width() * 1.0 - fixedWidth;
		double autoWidth = otherWidth / autoTotal;
		double maxRight = 0;
		//排序
		for (auto& it : GetControls()) {
			if (it->IsVisible() == false) continue;

			maxRight += it->Margin.Left;
			int_t height = it->GetFixedHeight();
			if (height == 0) {
				height = this->Height() - it->Margin.GetVSpace();
			}

			int_t y = it->Y();

			//if (!IsAutoHeight()) {
			if (ContentAlign == VAlign::Top) {
				y = it->Margin.Top;
			}
			else  if (ContentAlign == VAlign::Mid) {
				y = int_t((this->Height() * 1.0 - height) / 2.0f + 0.5);
			}
			else  if (ContentAlign == VAlign::Bottom) {
				y = this->Height() - it->Margin.Bottom - height;
			}
			//}
			if (it->GetFixedWidth() > 0 || it->IsAutoWidth()) {
				it->SetRect({ (int_t)maxRight ,y,it->GetFixedWidth() ,height });
				maxRight += it->Width();
			}
			else {
				it->SetRect({ (int_t)maxRight ,y,(int_t)autoWidth,height });
				maxRight += it->Width();
			}
			maxRight += it->Margin.Right;
			//统计上下文宽高
			int_t maxBottom = it->GetRect().GetBottom() + it->Margin.Bottom;
			if (maxBottom > contentHeight) {
				contentHeight = maxBottom;
			}
			this->SetContentSize({ (int_t)(maxRight + 0.5) ,contentHeight });
		}
	}
	HLayout::HLayout(Object* parentObject):Control(parentObject)
	{
	}
	HLayout::~HLayout()
	{
	}
};