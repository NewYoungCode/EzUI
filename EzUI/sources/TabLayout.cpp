#include "TabLayout.h"

namespace EzUI {
	TabLayout::TabLayout()
	{
	}
	TabLayout::~TabLayout()
	{
	}
	void TabLayout::Remove(Control* ctl)
	{
		size_t index = this->IndexOf(ctl);
		__super::Remove(ctl);
		int newIndex = this->GetControls().size() - 1;
		newIndex = newIndex < 0 ? 0 : newIndex;
		this->SetPageIndex(newIndex);
	}
	void TabLayout::OnLayout()
	{
		int pos = 0;
		for (auto i = GetControls().begin(); i != GetControls().end(); i++)
		{
			(*i)->SetRect(Rect(0, 0, Width(), Height()));
			(*i)->SetVisible(false);
			if (pos == _index) {
				(*i)->SetVisible(true);
				_index = pos;
			}
			pos++;
		}
		this->EndLayout();
	}

	void TabLayout::SetPageIndex(int index)
	{
		this->_index = index;
		this->TryPendLayout();
	}

	void TabLayout::SetPage(Control* ctl)
	{
#ifdef _DEBUG
		ASSERT(ctl->Parent == this);
#endif // _DEBUG
		this->_index = this->IndexOf(ctl);
		this->TryPendLayout();
	}

	Control* TabLayout::GetPage() {
		int pos = 0;
		for (auto& it : GetControls()) {
			if (pos == this->_index) {
				return it;
			}
			pos++;
		}
		return NULL;
	}
	int TabLayout::GetPageIndex()
	{
		return _index;
	}
};