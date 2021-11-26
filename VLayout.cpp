#include "VLayout.h"

VLayout::~VLayout()
{
}

void VLayout::Sort()
{
	int fixedHeight = 0;
	int fixedTotal = 0;
	int count = 0;//可见控件总数
	for (auto &it : _controls) {
		if (it->Visible == false)continue;
		count++;
		auto height = it->GetFixedHeight();
		if (height > 0) {
			fixedHeight += height;
			fixedTotal++;
		}
	}
	int autoTotal = count - fixedTotal;
	if (autoTotal == 0) {
		//return;
	}
	double otherHeight = Height()*1.0 - fixedHeight;
	double autoHeight = otherHeight / autoTotal;
	double maxBottom = 0;
	//排序
	for (auto &it : _controls) {
		if (it->Visible == false)continue;
		it->OnLayout({ Width(),Height() },false);//触发原始布局特性
		if (it->GetFixedHeight() > 0) {
			it->SetRect({ it->X(), (int)maxBottom,it->Width(), it->GetFixedHeight() });
			maxBottom += it->Height();
		}
		else {
			it->SetRect({ it->GetRect().X,(int)maxBottom,it->Width() ,(int)autoHeight });
			maxBottom += autoHeight;
		}
	}
}



void VLayout::AddControl(Control * ctl)
{
	Control::AddControl(ctl);
	if (ctl->Width() == 0 && ctl->X() == 0) {
		ctl->Dock = DockStyle::Horizontal;
	}
	if (ctl->Visible == true) {
		Sort();
	}
}

void VLayout::OnSize(const Size&size) {
	Sort();
}

void VLayout::RefreshLayout()
{
	Sort();
	Refresh();
}

