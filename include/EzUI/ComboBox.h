#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VListView.h"
#include "PopupWindow.h"
#include "HLayout.h"
#include "RadioButton.h"
#include "Animation.h"

namespace ezui {
	//简易的下拉列表框
	class UI_EXPORT ComboBox :public HLayout {
	private:
		//添加选项请使用AddOption
		virtual Control* AddChild(Control* childCtl)override;
		//移除选项请使用RemoveOption
		virtual void RemoveChild(Control* childCtl, bool freeCtrl)override;
		virtual void OnDpiChanged(const DpiChangedEventArgs& arg)override;
	private:
		//下拉列表窗口
		class DropDownWindow :public PopupWindow {
			ComboBox* m_ownerCtrl;
		public:
			virtual void OnShow() override;
			virtual void OnNotify(Control* sd, EventArgs& args)override;
			DropDownWindow(ComboBox* ownerCtl);
			virtual ~DropDownWindow();
		};
	private:
		void Init();
		//选择之后显示的文本框
		TextBox& m_textBox;
		//展开菜单的按钮
		CheckBox& m_UpDown;
		//存储选项的集合
		VListView& m_list;
		//已选中的值
		UIString m_value;
		//下拉动画
		Animation* m_ani;
		//选中的下标
		int m_selectedIndex = -1;
	public:
		//用户界面切换选项的时候发生的回调
		std::function<void(ComboBox* sender, int index)> SelectedChanged = NULL;
	protected:
		virtual void OnLayout()override;
		//当正在创建某项option的时候发生(重写此函数来达到DIY效果 (重写此函数来达到diy效果 返回的控件请正确设置绝对高度))
		virtual Control* OnCreateOption(const UIString& optionValue, const UIString& optionShowText, Image* icon);
	public:
		ComboBox(Object* ownerObject = NULL);
		virtual void SetAttribute(const UIString& attrKey, const UIString& attrValue)override;
		//添加一个选项
		Control* AddOption(const UIString& optionValue, const UIString& optionShowText, Image* icon = NULL);
		//移除一个选项
		void RemoveOption(const UIString& optionValue);
		//移除全部选项
		void RemoveAllOptions();
		//获取选中的value(返回optionValue)
		UIString GetValue();
		//选中某个optionValue
		bool SetValue(const UIString& optionValue, bool triggerChangedEvent = false);
		//获取当前被选中的下标值
		int GetSelectedIndex();
		//设置指定下标为选中状态
		bool SelectIndex(int index, bool triggerChangedEvent = false);
		//获取选中的文字
		UIString GetText();
		//获取承载选项的容器
		VListView* GetOptionPanel();
		virtual ~ComboBox();
	};
};
