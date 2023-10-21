#pragma once
#include "List.h"
namespace EzUI {

	void List::SetPageInfo(const std::list<Control*>& items, int pageSize)
	{
		this->pageIndex = 0;
		this->pageSize = pageSize;
		this->pageTotal = items.size() / pageSize + (items.size() % pageSize == 0 ? 0 : 1);
		this->items = items;
		this->NextPage();
	}

	void List::Clear() {
		__super::Clear();
	}

	void List::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		this->items.clear();
		this->itor = items.begin();
		this->pageIndex = 0;
		this->pageTotal = 0;
	}

	void List::NextPage()
	{
		if (items.size() <= 0)return;
		if (pageIndex > pageTotal)return;
		if (NextPaging && NextPaging(this) == false) {
			return;
		}
		if (pageIndex == 0) {
			itor = items.begin();
		}
		pageIndex++;
		int count = 0;
		while (itor != items.end())
		{
			this->Add(*itor);
			count++;
			itor++;
			if (count >= pageSize)break;
		}
		this->Invalidate();
	}
	List::List()
	{
	}
	List::~List()
	{
	}
};