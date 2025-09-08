#include "PagedListView.h"
namespace ezui {

	void PagedListView::SetPageInfo(const ControlCollection& items, int pageSize)
	{
		this->m_pageIndex = 0;
		this->m_pageSize = pageSize;
		this->m_pageTotal = items.size() / pageSize + (items.size() % pageSize == 0 ? 0 : 1);
		this->m_items = items;
		this->NextPage();
	}

	void PagedListView::GetPage(int pageIndex, ControlCollection* outCtls)
	{
		if (outCtls) {
			outCtls->clear();
			int beginIndex = (pageIndex - 1) * m_pageSize;
			int count = 0;
			for (int i = beginIndex; count < m_pageSize && i < m_items.size(); ++i)
			{
				outCtls->push_back(m_items[i]);
				++count;
			}
		}
	}

	void PagedListView::Clear() {
		__super::Clear();
	}

	void PagedListView::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		this->m_items.clear();
		this->m_pageIndex = 0;
		this->m_pageTotal = 0;
	}

	void PagedListView::NextPage()
	{
		if (m_items.size() <= 0)return;
		if ((m_pageIndex + 1) > m_pageTotal)return;
		if (NextPaging && NextPaging(this, (m_pageIndex + 1)) == false) {
			return;
		}
		++m_pageIndex;
		ControlCollection ctls;
		this->GetPage(m_pageIndex, &ctls);
		for (auto& it : ctls) {
			this->Add(it);
		}
		this->Invalidate();
	}

	PagedListView::PagedListView(Object* patentObject):Control(patentObject)
	{
	}
	PagedListView::~PagedListView()
	{
	}
};