#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VListView.h"
#include "PopupWindow.h"
#include "HLayout.h"

namespace ezui {
	//简易的下拉列表框
	class UI_EXPORT ComboBox :public HLayout {
	private:
		//添加选项请使用AddItem
		virtual Control* Add(Control* childCtl)override;
		//移除选项请使用RemoveItem
		virtual void Remove(Control* childCtl, bool freeCtrl)override;
	private:
		//下拉菜单选项
		class MenuContent :public PopupWindow {
		public:
			Control* m_hittestCtl;
			MenuContent(Control* ownerCtl, Control* hittestCtl);
			virtual void OnKillFocus(HWND wnd) override;
			virtual ~MenuContent();
		};
	private:
		//下拉菜单窗口
		MenuContent* m_menuWnd = NULL;
		//选择之后显示的文本框
		TextBox m_textBox;
		//展开菜单的按钮
		Label m_UpDown;

		VListView m_list;
		int m_index = -1;
		void Init();
	protected:
		virtual void OnLayout()override;
	public:
		ComboBox(Object* parentObject = NULL);
		//获取选中的文字
		UIString GetText();
		//获取选中的下标
		int GetCheck();
		//选中某个下标
		bool SetCheck(int pos);
		virtual ~ComboBox();
		//添加一个item并返回新item的下标
		int AddItem(const UIString& text);
		void RemoveItem(int index);
	};
};
