#include "VLayout.h"
namespace ezui {
	VLayout::VLayout(Object* parentObject) :Control(parentObject)
	{
	}
	void VLayout::SetAttribute(const UIString& key, const UIString& value)
	{
		if (key == "halign" || key == "align") {
			if (value == "left") {
				ContentAlign = HAlign::Left;
			}
			else if (value == "right") {
				ContentAlign = HAlign::Right;
			}
		}
		__super::SetAttribute(key, value);
	}
	VLayout::~VLayout()
	{
	}

	void VLayout::OnLayout()
	{
		int contentWidth = 0;

		int fixedHeight = 0;
		int fixedTotal = 0;
		int count = 0;//可见控件总数
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
			int x = it->X();

			if (width == 0) {
				//当控件未指定绝对宽度的时候
				width = this->Width() - it->Margin.GetHSpace();
				x = it->Margin.Left;
			}
			else {
				//当控件指定了绝对宽度的时候
				if (ContentAlign == HAlign::Left) {
					x = it->Margin.Left;
				}
				else  if (ContentAlign == HAlign::Center) {
					x = int((this->Width() * 1.0 - width) / 2.0f + 0.5);
				}
				else if (ContentAlign == HAlign::Right) {
					x = this->Width() - it->Margin.Right - width;
				}
			}

			if (it->GetFixedHeight() > 0 || it->IsAutoHeight()) {
				it->SetRect({ x, (int)maxBottom,width, it->GetFixedHeight() });
				maxBottom += it->Height();
			}
			else {
				it->SetRect({ x ,(int)maxBottom,width ,(int)autoHeight });
				maxBottom += it->Height();
			}
			maxBottom += it->Margin.Bottom;
			//统计上下文宽高
			int maxRight = it->GetRect().GetRight() + it->Margin.Right;
			if (maxRight > contentWidth) {
				contentWidth = maxRight;
			}
			this->SetContentSize({ contentWidth ,(int)(maxBottom + 0.5) });
		}
	}
};