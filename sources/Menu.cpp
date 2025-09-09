#include "Menu.h"

namespace ezui {
	UINT_PTR g_nextId = WM_APP + 1;
	inline UINT_PTR GenerateMenuId() {
		++g_nextId;
		return g_nextId;
	}

	Menu::Menu(Object* parentObj) : Object(parentObj)
	{
		m_hMenu = ::CreatePopupMenu(); // 创建空菜单
	}

	UINT_PTR Menu::Append(const UIString& text)
	{
		UINT_PTR id = GenerateMenuId();
		if (m_hMenu) {
			::AppendMenuW(m_hMenu, MF_STRING, id, text.unicode().c_str());
		}
		return id;
	}

	void Menu::Remove(UINT_PTR itemID)
	{
		if (!m_hMenu) return;
		int count = ::GetMenuItemCount(m_hMenu);
		for (int i = 0; i < count; ++i)
		{
			MENUITEMINFO mii = {};
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_ID;

			if (::GetMenuItemInfo(m_hMenu, i, TRUE, &mii)) {
				if (mii.wID == itemID) {
					::RemoveMenu(m_hMenu, i, MF_BYPOSITION);
					break;
				}
			}
		}
	}
	Menu::~Menu()
	{
		if (m_hMenu) {
			::DestroyMenu(m_hMenu);
		}
	}

	HMENU Menu::HMenu()
	{
		return m_hMenu;
	}

};