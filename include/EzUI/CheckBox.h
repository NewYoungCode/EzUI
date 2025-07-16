#pragma once
#include "Label.h"

namespace ezui {

	class UI_EXPORT CheckBox :
		public Label
	{
	private:
		bool m_checked = false;
	public:
		//选中样式
		ControlStyle CheckedStyle;
	protected:
		virtual ControlStyle& GetStyle(const ControlState& _state)override;
		virtual ControlStyle& GetDefaultStyle()override;
		virtual void OnMouseClick(const MouseEventArgs& arg)override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
	public:
		//选中状态发送变化的回调函数
		std::function<void(CheckBox* sender, bool checked)> CheckedChanged = NULL;
		CheckBox();
		CheckBox(Control* parentObject);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		//设置选中状态
		virtual void SetCheck(bool checked);
		//获取选中状态
		virtual bool GetCheck();
		virtual~CheckBox();
	};

};