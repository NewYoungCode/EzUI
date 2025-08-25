#pragma once
#include "HListView.h"
#include "VListView.h"
namespace ezui {
	//树形菜单 还在设计中
	class UI_EXPORT TreeView :public HListView {
	private:
		VListView m_vList;
	public:
		TreeView(Object* parentObj = NULL);
		void AddNode(const UIString& nodeName);
		virtual ~TreeView();
	};
};