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
		//ҳ����Ҫ������һҳ��ʱ����
		std::function<bool(List*, int)> NextPaging = NULL;
		void SetPageInfo(const std::vector<Control*>& items, int pageSize);
		/// <summary>
		/// ��ȡĳҳ��item����
		/// </summary>
		/// <param name="index">1~N</param>
		/// <param name="outCtls">�������</param>
		void GetPage(int index, std::vector<Control*>* outCtls);
		virtual void NextPage();
		virtual void Clear() override;
		virtual void Clear(bool freeChilds) override;
		List();
		virtual ~List();
	};
};