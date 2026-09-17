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
	class EZUI_API PagedListView :
		public Control
	{
		friend class VListView;
		friend class HListView;
		friend class TileListView;
	private:
		int m_pageIndex;
		int m_pageTotal;
		int m_pageSize;
		ControlCollection& m_items;
	private:
		//不允许外部直接使用PagedListView(此类为接口类)
		PagedListView(Object* ownerObject = NULL);
	public:
		//页面需要加载下一页的时候发生
		std::function<bool(PagedListView*, int)> NextPaging;

		void SetPageInfo(const ControlCollection& items, int pageSize);

		/// <summary>
		/// 获取某页的item集合
		/// </summary>
		/// <param name="index">1~N</param>
		/// <param name="outCtls">输出集合</param>
		/// 
		void GetPage(int index, ControlCollection* outCtls);

		//加载下一页数据
		virtual void NextPage();

		/// 移除全部子控件；deleteChildren 为 true 时同时立即删除这些控件。
		virtual void RemoveAll(bool deleteChildren = false) EZUI_OVERRIDE;

		virtual ~PagedListView();
	};
};
