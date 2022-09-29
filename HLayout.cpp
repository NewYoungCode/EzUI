#include "HLayout.h"
namespace EzUI {
	void HLayout::ResumeLayout()
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
			if (it->GetFixedWidth() > 0) {
				it->SetRect({ (int)maxRight ,it->GetRect().Y,it->GetFixedWidth() ,it->GetRect().Height });
				maxRight += it->Width();
			}
			else {
				it->SetRect({ (int)maxRight ,it->GetRect().Y,(int)autoWidth,it->Height() });
				maxRight += autoWidth;
			}
		}
		this->PendLayout = false;
	}
	void HLayout::AddControl(Control* ctl)
	{
		Control::AddControl(ctl);
		if (ctl->Height() == 0 && ctl->Y() == 0) {
			ctl->Dock = DockStyle::Vertical;
		}
		if (ctl->Visible == true) {
			ResumeLayout();
		}
		this->PendLayout = true;
	}
	ControlIterator HLayout::RemoveControl(Control* ctl)
	{
		ControlIterator it = __super::RemoveControl(ctl);
		PendLayout = true;
		return it;
	}
	bool HLayout::OnSize(const Size& size) {
		if (__super::OnSize(size)) {
			ResumeLayout();
			return true;
		}
		return false;
	}
};