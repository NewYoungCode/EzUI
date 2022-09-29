#include "VLayout.h"
namespace EzUI {


	VLayout::~VLayout()
	{
	}

	void VLayout::ResumeLayout()
	{
		int fixedHeight = 0;
		int fixedTotal = 0;
		int count = 0;//可见控件总数
		for (auto& it : _controls) {
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
		double otherHeight = Height() * 1.0 - fixedHeight;
		double autoHeight = otherHeight / autoTotal;
		double maxBottom = 0;
		//排序
		for (auto& it : _controls) {
			if (it->Visible == false)continue;
			if (it->GetFixedHeight() > 0) {
				it->SetRect({ it->X(), (int)maxBottom,it->Width(), it->GetFixedHeight() });
				maxBottom += it->Height();
			}
			else {
				it->SetRect({ it->GetRect().X,(int)maxBottom,it->Width() ,(int)autoHeight });
				maxBottom += autoHeight;
			}
		}
		this->PendLayout = false;
	}

	void VLayout::AddControl(Control* ctl)
	{
		__super::AddControl(ctl);
		PendLayout = true;
		if (ctl->Width() == 0 && ctl->X() == 0) {
			ctl->Dock = DockStyle::Horizontal;
		}
		if (ctl->Visible == true) {
			ResumeLayout();
		}
	}

	ControlIterator VLayout::RemoveControl(Control* ctl) {
		ControlIterator it = __super::RemoveControl(ctl);
		PendLayout = true;
		return it;
	}

	bool VLayout::OnSize(const Size& sz) {
		if (__super::OnSize(sz)) {
			ResumeLayout();
			return true;
		}
		return false;
	}

};