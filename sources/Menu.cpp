#include "Menu.h"

namespace ezui {
	UINT_PTR _dui_menu_index = 0;
	Menu::Menu()
	{
		m_hMenu = ::CreatePopupMenu();
	}

	Menu::~Menu()
	{
	}

	Menu::Menu(const UIString& text) {
		m_hMenu = ::CreatePopupMenu();
		m_text = text;
	}

	void Menu::Append(Menu* menu)
	{
		::AppendMenuW(m_hMenu, MF_POPUP, (UINT_PTR)menu->m_hMenu, menu->m_text.unicode().c_str());
		//return (UINT_PTR)menu->_hMenu;
	}


	UINT_PTR Menu::Append(const UIString& text) {
		::AppendMenuW(m_hMenu, MF_POPUP, _dui_menu_index, text.unicode().c_str());
		m_list.push_back(_dui_menu_index);
		auto temp = _dui_menu_index;
		++_dui_menu_index;
		return temp;
	}

	void Menu::Remove(UINT_PTR id) {
		RemoveMenu(m_hMenu, (UINT)id, MF_BYCOMMAND);
		for (auto itor = m_list.begin(); itor < m_list.end(); ++itor) {
			if (id == *itor) {
				m_list.erase(itor);
				break;
			}
		}
	}

	void Menu::Clear() {
		for (auto it : m_list) {
			Remove(it);
		}
	}

	void Menu::SetText(const UIString& text)
	{
		m_text = text;
	}

};