#include "TabLayout.h"

namespace EzUI {
	TabLayout::TabLayout()
	{
	}
	TabLayout::~TabLayout()
	{
	}

	void TabLayout::ResumeLayout()
	{
		__super::ResumeLayout();

		SetPageIndex(_index);
	}

	void TabLayout::SetPageIndex(int index)
	{
		int pos = 0;
		for (auto i = _controls.begin(); i != _controls.end(); i++)
		{
			if (pos == index) {
				(*i)->Visible = true;
				(*i)->SetRect(Rect(0, 0, Width(), Height()));
				_index = pos;
			}
			else {
				(*i)->Visible = false;
			}
			pos++;
		}
	}

	void TabLayout::AddControl(Control* ctl) {
		__super::AddControl(ctl);
		if (ctl->Visible == true) {
			//ResumeLayout();
		}
	}

	ControlIterator TabLayout::RemoveControl(Control* ctl)
	{
		ControlIterator it = __super::RemoveControl(ctl);
		return it;
	}

	int TabLayout::PageIndex()
	{
		return _index;
	}

	bool TabLayout::OnSize(const Size& sz)
	{
		if (__super::OnSize(sz)) {
			ResumeLayout();
			return true;
		}
		return false;

	}
};