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

		VListView _list;
		int_t _index = -1;
		void Init();
	protected:
		virtual void OnLayout()override;
	public:
		ComboBox();
		//获取选中的文字
		UIString GetText();
		//获取选中的下标
		int_t GetCheck();
		//选中某个下标
		bool SetCheck(int_t pos);
		virtual ~ComboBox();
		int_t AddItem(const UIString& text);
		void RemoveItem(int_t index);
	};
};
