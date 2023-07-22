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

			(*i)->SetVisible(false);
			if (pos == index) {
				(*i)->SetVisible(true);
				_index = pos;
			}
			pos++;
		}
		this->EndLayout();
	}

	int TabLayout::GetPageIndex()
	{
		return _index;
	}
};