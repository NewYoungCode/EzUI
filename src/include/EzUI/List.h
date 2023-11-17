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
		std::vector<Control*> items;
	public:
		//页面需要加载下一页的时候发生
		std::function<bool(List*, int)> NextPaging = NULL;
		void SetPageInfo(const std::vector<Control*>& items, int pageSize);
		/// <summary>
		/// 获取某页的item集合
		/// </summary>
		/// <param name="index">1~N</param>
		/// <param name="outCtls">输出集合</param>
		void GetPage(int index, std::vector<Control*>* outCtls);
		virtual void NextPage();
		virtual void Clear() override;
		virtual void Clear(bool freeChilds) override;
		List();
		virtual ~List();
	};
};