#include "TabLayout.h"

namespace EzUI {
	TabLayout::TabLayout()
	{
	}
	TabLayout::~TabLayout()
	{
	}

	void TabLayout::OnLayout()
	{

		SetPageIndex(_index);
	}

	void TabLayout::SetPageIndex(int index)
	{
		this->TryPendLayout();
		int pos = 0;
		for (auto i = GetControls().begin(); i != GetControls().end(); i++)
		{
			(*i)->SetRect(Rect(0, 0, Width(), Height()));
			if (pos == index) {
				(*i)->Visible = true;
				_index = pos;
			}
			else {
				(*i)->Visible = false;
			}
			pos++;
		}
		this->EndLayout();
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

	void TabLayout::OnSize(const Size& sz)
	{
		__super::OnSize(sz);
		ResumeLayout();
	}
};