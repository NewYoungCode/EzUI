#pragma once
#include "EzUI.h"
namespace EzUI {

	class UI_EXPORT Menu
	{
	public:
		HMENU _hMenu;
		EString _text;
		std::vector<UINT_PTR> _list;
		std::function<void(UINT_PTR)> Callback = NULL;
	public:
		Menu();
		virtual ~Menu();
		Menu(const EString& text);
		void Append(Menu* menu);
		UINT_PTR Append(const EString& text);
		void Remove(UINT_PTR id);
		void Clear();
		void SetText(const EString& text);
	};

};