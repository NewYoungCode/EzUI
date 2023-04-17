#include "TabLayout.h"

namespace EzUI {
	TabLayout::TabLayout()
	{
	}
	TabLayout::TabLayout(Control* parent) :Control(parent)
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
			(*i)->Visible = false;
			if (pos == index) {
				(*i)->Visible = true;
				_index = pos;
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

	int TabLayout::GetPageIndex()
	{
		return _index;
	}

	void TabLayout::OnSize(const Size& sz)
	{
		__super::OnSize(sz);
		ResumeLayout();
	}
};