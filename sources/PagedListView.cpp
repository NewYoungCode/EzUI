#include "PagedListView.h"
namespace EzUI {

	void PagedListView::SetPageInfo(const Controls& items, int_t pageSize)
	{
		this->_pageIndex = 0;
		this->_pageSize = pageSize;
		this->_pageTotal = items.size() / pageSize + (items.size() % pageSize == 0 ? 0 : 1);
		this->_items = items;
		this->NextPage();
	}

	void PagedListView::GetPage(int_t pageIndex, Controls* outCtls)
	{
		if (outCtls) {
			outCtls->clear();
			int_t beginIndex = (pageIndex - 1) * _pageSize;
			int_t count = 0;
			for (int_t i = beginIndex; count < _pageSize && i < _items.size(); ++i)
			{
				outCtls->push_back(_items[i]);
				++count;
			}
		}
	}

	void PagedListView::Clear() {
		__super::Clear();
	}

	void PagedListView::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		this->_items.clear();
		this->_pageIndex = 0;
		this->_pageTotal = 0;
	}

	void PagedListView::NextPage()
	{
		if (_items.size() <= 0)return;
		if ((_pageIndex + 1) > _pageTotal)return;
		if (NextPaging && NextPaging(this, (_pageIndex + 1)) == false) {
			return;
		}
		++_pageIndex;
		Controls ctls;
		this->GetPage(_pageIndex, &ctls);
		for (auto& it : ctls) {
			this->Add(it);
		}
		this->Invalidate();
	}
	PagedListView::PagedListView()
	{
	}
	PagedListView::~PagedListView()
	{
	}
};