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
		virtual Control* AddChild(Control* childCtrl, bool takeOwnership = false)EZUI_OVERRIDE;
		using HLayout::RemoveChild;
		using HLayout::RemoveAll;
		using HLayout::InsertChildAt;
		using HLayout::SwapChild;
	protected:
		virtual void OnDpiChanged(DpiChangedEventArgs* arg)EZUI_OVERRIDE;
		//获取内部用于显示当前选项内容的控件，子类可用于自定义显示行为(默认返回TextBox*类型)
		virtual Control* GetDisplayControl();
	private:
		//下拉列表窗口
		class DropDownWindow :public PopupWindow {
			ComboBox* m_ownerCtrl;
		public:
			virtual void OnNotify(Control* sender, EventArgs* args)EZUI_OVERRIDE;
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
		int m_selectedIndex;
		//下拉框是否已展开
		bool m_expanded;
	public:
		//(下拉选项)其实就是一个控件
		typedef Control Option;
		//用户界面切换选项的时候发生的回调
		std::function<void(ComboBox* sender, int index)> SelectedChanged;
	protected:
		virtual void OnLayout()EZUI_OVERRIDE;
		//当正在创建某项option的时候发生(重写此函数来达到DIY效果 (重写此函数来达到diy效果 返回的控件请正确设置绝对高度 请自行管理Option*的释放))
		virtual Option* OnCreateOption(const UIString& optionValue, const UIString& optionShowText, Image* icon);
		// 下拉框展开前调用。
		// 此时 dropWindow 的默认宽高已由内部完成计算，
		// 可在此调整最终窗口大小、弹出位置或展开动画。
		virtual void OnDropDown(Window* dropWindow);
		//这是一个可重写的虚函数，允许子类自定义箭头的绘制样式。
		//当控件需要重绘箭头时，会调用此函数。
		virtual void OnPaintDropDownArrow(Control* btnArrow, PaintEventArgs* args);
	public:
		ComboBox(Object* ownerObject = NULL);

		// Supported attributes:
		// valign | halign | align: top | bottom | mid | left | right | center (forwarded to TextBox)
		// Note: value/text/options attributes are not handled in SetAttribute
		// All HLayout attributes are also supported
		virtual void SetAttribute(const UIString& attrKey, const UIString& attrValue)EZUI_OVERRIDE;

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
