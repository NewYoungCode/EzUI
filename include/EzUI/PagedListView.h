#pragma once
#include "Control.h"

namespace ezui {
	/**
 * 是一个分页显示控件集合的容器控件
 *
 * 它支持对一批子控件进行分页管理，例如:
 * - 显示每页固定数量的控件;
 * - 支持翻页;
 * - 当控件到达末页时需要手动调用NextPage进行加载下一页;
 *
 * 子类:VList/HList/TileList 继承使其具备分页管理的能力
 */
	class UI_EXPORT PagedListView :
		public Control
	{
	private:
		int_t _pageIndex = 0;
		int_t _pageTotal = 0;
		int_t _pageSize = 0;
		Controls _items;
	public:
		//页面需要加载下一页的时候发生
		std::function<bool(PagedListView*, int_t)> NextPaging = NULL;
		void SetPageInfo(const Controls& items, int_t pageSize);
		/// <summary>
		/// 获取某页的item集合
		/// </summary>
		/// <param name="index">1~N</param>
		/// <param name="outCtls">输出集合</param>
		void GetPage(int_t index, Controls* outCtls);
		virtual void NextPage();
		virtual void Clear() override;
		virtual void Clear(bool freeChilds) override;
		PagedListView();
		virtual ~PagedListView();
	};
};