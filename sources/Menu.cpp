#include "Menu.h"

namespace ezui {
	UINT_PTR _dui_menu_index = 0;
	Menu::Menu()
	{
		_hMenu = ::CreatePopupMenu();
	}

	Menu::~Menu()
	{
	}

	Menu::Menu(const UIString& text) {
		_hMenu = ::CreatePopupMenu();
		_text = text;
	}

	void Menu::Append(Menu* menu)
	{
		::AppendMenuW(_hMenu, MF_POPUP, (UINT_PTR)menu->_hMenu, menu->_text.unicode().c_str());
		//return (UINT_PTR)menu->_hMenu;
	}


	UINT_PTR Menu::Append(const UIString& text) {
		::AppendMenuW(_hMenu, MF_POPUP, _dui_menu_index, text.unicode().c_str());
		_list.push_back(_dui_menu_index);
		auto temp = _dui_menu_index;
		++_dui_menu_index;
		return temp;
	}

	void Menu::Remove(UINT_PTR id) {
		RemoveMenu(_hMenu, (UINT)id, MF_BYCOMMAND);
		for (auto itor = _list.begin(); itor < _list.end(); ++itor) {
			if (id == *itor) {
				_list.erase(itor);
				break;
			}
		}
	}

	void Menu::Clear() {
		for (auto it : _list) {
			Remove(it);
		}
	}

	void Menu::SetText(const UIString& text)
	{
		_text = text;
	}

};