#include "List.h"
namespace EzUI {

	void List::SetPageInfo(const std::vector<Control*>& items, int pageSize)
	{
		this->pageIndex = 0;
		this->pageSize = pageSize;
		this->pageTotal = items.size() / pageSize + (items.size() % pageSize == 0 ? 0 : 1);
		this->items = items;
		this->NextPage();
	}

	void List::GetPage(int pageIndex, std::vector<Control*>* outCtls)
	{
		if (outCtls) {
			outCtls->clear();
			size_t beginIndex = (pageIndex - 1) * pageSize;
			size_t count = 0;
			for (size_t i = beginIndex; count < pageSize && i < items.size(); ++i)
			{
				outCtls->push_back(items[i]);
				++count;
			}
		}
	}

	void List::Clear() {
		__super::Clear();
	}

	void List::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		this->items.clear();
		this->pageIndex = 0;
		this->pageTotal = 0;
	}

	void List::NextPage()
	{
		if (items.size() <= 0)return;
		if ((pageIndex + 1) > pageTotal)return;
		if (NextPaging && NextPaging(this, (pageIndex + 1)) == false) {
			return;
		}
		++pageIndex;
		std::vector<Control*> ctls;
		this->GetPage(pageIndex, &ctls);
		for (auto& it : ctls) {
			this->Add(it);
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