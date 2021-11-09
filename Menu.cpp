#include "Menu.h"
UINT_PTR _dui_menu_index = 0;
Menu::Menu()
{
	_hMenu = ::CreatePopupMenu();
}

Menu::~Menu()
{
}

Menu::Menu(const EString & text) {
	_hMenu = ::CreatePopupMenu();
	_text = text;
}

void Menu::Append(Menu * menu)
{
	::AppendMenuW(_hMenu, MF_POPUP, (UINT_PTR)menu->_hMenu, menu->_text.utf16().c_str());
	//return (UINT_PTR)menu->_hMenu;
}


UINT_PTR Menu::Append(const EString&text) {
	::AppendMenuW(_hMenu, MF_POPUP, _dui_menu_index, text.utf16().c_str());
	_list.push_back(_dui_menu_index);
	auto temp = _dui_menu_index;
	_dui_menu_index++;
	return temp;
}

void Menu::Remove(UINT_PTR id) {
	RemoveMenu(_hMenu, (UINT)id, MF_BYCOMMAND);
	for (auto it = _list.begin(); it < _list.end(); it++) {
		if (id == *it) {
			_list.erase(it);
			break;
		}
	}
}

void Menu::Clear() {
	for (auto it : _list) {
		Remove(it);
	}
}

void Menu::SetText(const EString&text)
{
	_text = text;
}

void Menu::SetClickCallBack(const std::function<void(UINT_PTR)>&callback) {
	_callback = callback;
}