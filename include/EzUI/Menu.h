#pragma once
#include "EzUI.h"

namespace ezui {
	//菜单类
	class UI_EXPORT Menu :public Object
	{
	private:
		HMENU m_hMenu = NULL;
	public:
		//菜单子项被选点击的回调事件 UINT:子项ID
		std::function<void(UINT_PTR)> MouseClick = NULL;
		Menu(Object* ownerObject = NULL);
		virtual ~Menu();
		HMENU HMenu();
		UINT_PTR Append(const UIString& text);
		void Remove(const UINT_PTR id);
	};

};