#include "TabLayout.h"
TabLayout::TabLayout()
{
}
TabLayout::~TabLayout()
{
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
	SetPageIndex(_index);
}

int TabLayout::PageIndex()
{
	return _index;
}

void TabLayout::RefreshLayout()
{
	Refresh();
}

void TabLayout::OnSize(const Size& sz)
{
	__super::OnSize(sz);
	SetPageIndex(_index);
}
