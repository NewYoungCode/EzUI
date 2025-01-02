#include "List.h"
namespace EzUI {

	void List::SetPageInfo(const Controls& items, int pageSize)
	{
		this->_pageIndex = 0;
		this->_pageSize = pageSize;
		this->_pageTotal = items.size() / pageSize + (items.size() % pageSize == 0 ? 0 : 1);
		this->_items = items;
		this->NextPage();
	}

	void List::GetPage(int pageIndex, Controls* outCtls)
	{
		if (outCtls) {
			outCtls->clear();
			size_t beginIndex = (pageIndex - 1) * _pageSize;
			size_t count = 0;
			for (size_t i = beginIndex; count < _pageSize && i < _items.size(); ++i)
			{
				outCtls->push_back(_items[i]);
				++count;
			}
		}
	}

	void List::Clear() {
		__super::Clear();
	}

	void List::Clear(bool freeChilds) {
		__super::Clear(freeChilds);
		this->_items.clear();
		this->_pageIndex = 0;
		this->_pageTotal = 0;
	}

	void List::NextPage()
	{
		if (_items.size() <= 0)return;
		if ((_pageIndex + 1) > _pageTotal)return;
		if (NextPaging && NextPaging(this, (_pageIndex + 1)) == false) {
			return;
		}
		++_pageIndex;
		std::vector<Control*> ctls;
		this->GetPage(_pageIndex, &ctls);
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