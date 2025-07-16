#pragma once
#include "EzUI.h"

namespace ezui {

	class UI_EXPORT Menu
	{
	public:
		HMENU m_hMenu;
		UIString m_text;
		std::vector<UINT_PTR> m_list;
		std::function<void(UINT_PTR)> Callback = NULL;
	public:
		Menu();
		virtual ~Menu();
		Menu(const UIString& text);
		void Append(Menu* menu);
		UINT_PTR Append(const UIString& text);
		void Remove(UINT_PTR id);
		void Clear();
		void SetText(const UIString& text);
	};

};