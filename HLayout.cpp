#include "HLayout.h"

namespace EzUI {

	void HLayout::Sort()
	{
		int fixedWidth = 0;
		int fixedTotal = 0;
		int count = 0;//可见控件总数
		for (auto it : _controls) {
			if (it->Visible == false) continue;
			count++;
			auto width = it->GetFixedWidth();
			if (width > 0) {
				fixedWidth += width;
				fixedTotal++;
			}
		}
		int autoTotal = count - fixedTotal;
		if (autoTotal == 0) {
			//return;
		}
		double otherWidth = _rect.Width * 1.0 - fixedWidth;
		double autoWidth = otherWidth / autoTotal;
		double maxRight = 0;
		//排序
		for (auto& it : _controls) {
			if (it->Visible == false) continue;
			it->OnLayout({ Width(),Height() }, false);
			if (it->GetFixedWidth() > 0) {
				it->SetRect({ (int)maxRight ,it->GetRect().Y,it->GetFixedWidth() ,it->GetRect().Height });
				maxRight += it->Width();
			}
			else {
				it->SetRect({ (int)maxRight ,it->GetRect().Y,(int)autoWidth,it->Height() });
				maxRight += autoWidth;
			}
		}
	}

	void HLayout::AddControl(Control* ctl)
	{
		Control::AddControl(ctl);
		if (ctl->Height() == 0 && ctl->Y() == 0) {
			ctl->Dock = DockStyle::Vertical;
		}
		if (ctl->Visible == true) {
			Sort();
		}
	}

	void HLayout::OnSize(const Size& size) {
		Sort();
	}

	void HLayout::RefreshLayout()
	{
		Sort();
		Refresh();
	}
};