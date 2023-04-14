#pragma once
#include "EzUI.h"
namespace EzUI {

	class UI_EXPORT  Menu
	{
	public:
		std::function<void(UINT_PTR)> _callback = NULL;
		HMENU _hMenu;
		EString _text;
		std::vector<UINT_PTR> _list;
	public:
		Menu();
		virtual ~Menu();
		Menu(const EString& text);
		void Append(Menu* menu);
		UINT_PTR Append(const EString& text);
		void Remove(UINT_PTR id);
		void Clear();
		void SetText(const EString& text);
		void SetClickCallBack(const std::function<void(UINT_PTR)>& callback);
	};

};