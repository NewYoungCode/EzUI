#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT List :
		public Control
	{
	private:
		int pageIndex = 0;
		int pageTotal = 0;
		int pageSize = 0;
		std::list<Control*> items;
		std::list<Control*>::iterator itor;
	public:
		//页面需要加载下一页的时候发生
		std::function<bool(List*)> NextPaging = NULL;
		void SetPageInfo(const std::list<Control*>& items, int pageSize);
		virtual void NextPage();
		virtual void Clear() override;
		virtual void Clear(bool freeChilds) override;
		List();
		virtual ~List();
	};
};