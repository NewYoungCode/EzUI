#pragma once
#include "EzUI.h"
#include "VLayout.h"
#include "HLayout.h"
#include "Label.h"
#include "LayeredWindow.h"
#include "VListView.h"

namespace ezui {
	//使用windows自带菜单封装
	class UI_EXPORT SystemMenu :public Object
	{
	private:
		HMENU m_hMenu = NULL;
	public:
		//菜单子项被选点击的回调事件 UINT:子项ID
		std::function<void(UINT_PTR)> MenuClick = NULL;
		SystemMenu(Object* ownerObject = NULL);
		virtual ~SystemMenu();
		HMENU HMenu();
		//使用文字创建单个子项
		UINT_PTR Append(const UIString& text);
		//添加子菜单
		HMENU Append(SystemMenu* subMenu, const UIString& subMenuText);
		//创建一个二级菜单并插入
		SystemMenu* CreateMenu(const UIString& subMenuText);
		/// 添加分隔线
		void AddSeparator();
		//移除单个子项
		void Remove(UINT_PTR id);
		//移除子菜单
		void Remove(SystemMenu* menu);
		//显示菜单
		void Show(HWND ownerHWnd, Point lication);
	};


	//框架层面的多级菜单(如需自定义美观的菜单请使用此类)
	class UI_EXPORT PopupMenu :public LayeredWindow {
	private:
		VListView& m_menuPanel;
		PopupMenu* m_subMenu = NULL;
		PopupMenu* m_ownerMenu = NULL;
		std::map<Control*, PopupMenu*> m_subMenus;
	protected:
		virtual void OnNotify(Control* sd, EventArgs& args)override;
		virtual void OnKillFocus(HWND hWnd)override;
		virtual void OnMouseLeave()override;
		virtual void OnShow()override;
		virtual void OnHide()override;
	protected:
		/// 创建并返回一个菜单项控件，可设置文本、图标及是否为子菜单 ,如需重写此函数，请给返回的控件设置绝对宽高，以便自动计算弹出菜单大小
		/// @param text 菜单项显示文本
		/// @param icon 菜单项图标（可为 nullptr）
		/// @param isSubMenu 是否为子菜单项
		/// @return 返回创建的控件指针 (返回的控件请正确设置绝对宽高)
		virtual Control* OnCreateItem(const UIString& text, Image* icon, bool isSubMenu);
	public:
		//菜单子项被选点击的回调事件 返回点击的控件
		std::function<void(Control*)> MenuClick = NULL;
	public:
		PopupMenu(Object* ownerObj = NULL);
		virtual ~PopupMenu();
		//获取到根菜单(关闭根菜单则整个弹出菜单都会关闭)
		PopupMenu* GetRootMenu();
		Control* Append(const UIString& text, Image* icon = NULL);
		Control* Append(PopupMenu* subMenu, const UIString& subMenuText, Image* icon = NULL);

		/// 创建并插入一个子菜单
		/// @tparam T 子菜单类型，默认为 PopupMenu
		/// @param subMenuText 子菜单标题
		/// @param icon 子菜单图标（可选）
		/// @return 新创建的子菜单指针
		template<typename T = PopupMenu>
		T* CreateMenu(const UIString& subMenuText, Image* icon = NULL) {
			T* sub = new T(this);
			this->Append(sub, subMenuText, icon);
			return sub;
		}

		virtual void Show()override;
		void Remove(PopupMenu* subMenu);
	};
};