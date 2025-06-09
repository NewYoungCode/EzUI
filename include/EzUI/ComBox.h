#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VList.h"
#include "PopupWindow.h"
#include "HLayout.h"

namespace EzUI {
	//简易的下拉列表框
	class UI_EXPORT ComBox :public HLayout {
	private:
		//下拉菜单选项
		class MenuContent :public PopupWindow {
		public:
			Control* _hittestCtl;
			MenuContent(Control* ownerCtl, Control* hittestCtl);
			virtual void OnKillFocus(HWND wnd) override;
			virtual ~MenuContent();
		};
	private:
		//下拉菜单窗口
		MenuContent* _menuWnd = NULL;
		//选择之后显示的文本框
		TextBox _textBox;
		//展开菜单的按钮
		Label _UpDown;

		VList _list;
		int_t _index = -1;
		void Init();
	protected:
		virtual void OnLayout()override;
	public:
		ComBox();
		EString GetText();
		int_t GetCheck();
		bool SetCheck(int_t pos);
		virtual ~ComBox();
		int_t AddItem(const EString& text);
		void RemoveItem(int_t index);
	};
};
