#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VListView.h"
#include "window/PopupWindow.h"
#include "HLayout.h"
#include "RadioButton.h"
#include "core/Animation.h"

namespace ezui {
	//简易的下拉列表框
	class EZUI_API ComboBox :public HLayout {
	private:
		//添加选项请使用AddOption
		virtual Control* AddChild(Control* childCtrl, bool autoDelete = false)override;
		using HLayout::RemoveChild;
		using HLayout::RemoveAll;
		using HLayout::InsertChildAt;
		using HLayout::SwapChild;
	protected:
		virtual void OnDpiChanged(DpiChangedEventArgs* arg)override;
	private:
		//下拉列表窗口
		class DropDownWindow :public PopupWindow {
			ComboBox* m_ownerCtrl;
		public:
			virtual void OnNotify(Control* sender, EventArgs* args)override;
			DropDownWindow(ComboBox* ownerCtrl);
			virtual ~DropDownWindow();
		};
	private:
		void Init();
		bool ApplySelection(Control* option, int index, bool triggerChangedEvent);
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
		//下拉框是否已展开
		bool m_expanded = false;
	public:
		//(下拉选项)其实就是一个控件
		using Option = Control;
		//用户界面切换选项的时候发生的回调
		std::function<void(ComboBox* sender, int index)> SelectedChanged = NULL;
	protected:
		virtual void OnLayout()override;
		//当正在创建某项option的时候发生(重写此函数来达到DIY效果 (重写此函数来达到diy效果 返回的控件请正确设置绝对高度 请自行管理Option*的释放))
		virtual Option* OnCreateOption(const UIString& optionValue, const UIString& optionShowText, Image* icon);
		//下拉框展开时会调用此函数(通常可以在此函数中设置窗口大小/弹出位置/下拉动画)
		virtual void OnDropDown(Window* dropWindow);
		//这是一个可重写的虚函数，允许子类自定义箭头的绘制样式。
		//当控件需要重绘箭头时，会调用此函数。
		virtual void OnPaintDropButton(Control* btnArrow, PaintEventArgs* args);
	public:
		ComboBox(Object* ownerObject = NULL);

		// Supported attributes:
		// valign | halign | align: top | bottom | mid | left | right | center (forwarded to TextBox)
		// Note: value/text/options attributes are not handled in SetAttribute
		// All HLayout attributes are also supported
		virtual void SetAttribute(const UIString& attrKey, const UIString& attrValue)override;

		// 设置占位符文本
		void SetPlaceholderText(const UIString& text);

		//添加一个选项
		Option* AddOption(const UIString& optionValue, const UIString& optionShowText, Image* icon = NULL);

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

		//下来框是否已展开
		bool IsExpanded();

		//设置选中的文字(自动寻找对应的optionValue并选中)
		void SetText(const UIString& text, bool triggerChangedEvent = false);

		//获取承载选项的容器
		VListView* GetOptionPanel();

		virtual ~ComboBox();
	};
};
