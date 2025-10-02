#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VListView.h"
#include "PopupWindow.h"
#include "HLayout.h"
#include "RadioButton.h"

namespace ezui {
	//简易的下拉列表框
	class UI_EXPORT ComboBox :public HLayout {
	public:
		//作为下拉框选项专用
		using Option = RadioButton;
	private:
		//添加选项请使用AddItem
		virtual Control* AddChild(Control* childCtl)override;
		//移除选项请使用RemoveItem
		virtual void RemoveChild(Control* childCtl, bool freeCtrl)override;
	private:
		//下拉菜单选项
		class MenuContent :public PopupWindow {
		public:
			Control* m_ownerCtrl;
			Control* m_hittestCtl;
			MenuContent(Control* ownerCtl, Control* hittestCtl);
			virtual void OnKillFocus(HWND wnd) override;
			virtual void Show() override;
			virtual ~MenuContent();
		};
	private:
		void Init();
		//下拉菜单窗口
		MenuContent* m_menuWnd = NULL;
		//选择之后显示的文本框
		TextBox m_textBox;
		//展开菜单的按钮
		Label m_UpDown;
		//存储选项的集合
		VListView m_list;
		//已选中的值
		UIString m_value;
	public:
		//用户界面切换选项的时候发生的回调
		std::function<void(const UIString&)> ValueChanged = NULL;
	protected:
		virtual void OnLayout()override;
		void BindClick(Control* ctrl);
	public:
		ComboBox(Object* ownerObject = NULL);
		virtual void SetAttribute(const UIString& attrKey, const UIString& attrValue)override;
		//添加一个选项
		ComboBox::Option* AddOption(const UIString& optionValue, const UIString& optionShowText);
		//移除一个选项
		void RemoveOption(const UIString& optionValue);
		//移除全部选项
		void RemoveAllOptions();
		//获取选中的value(返回optionValue)
		UIString GetValue();
		//选中某个optionValue
		bool SetValue(const UIString& optionValue);
		//获取选中的文字
		UIString GetText();
		//获取承载选项的容器
		VListView* GetOptionPanel();
		virtual ~ComboBox();
	};
};
