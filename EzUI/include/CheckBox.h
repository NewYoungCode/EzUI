#pragma once
#include "Label.h"

namespace EzUI {

	class UI_EXPORT CheckBox :
		public Label
	{
	private:
		bool _checked = false;
	public:
		ControlStyle CheckedStyle;//选中样式
	protected:
		virtual ControlStyle& GetStyle(const ControlState& _state)override;
		virtual ControlStyle& GetDefaultStyle()override;
		virtual void OnMouseClick(const MouseEventArgs& arg)override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
	public:
		std::function<void(CheckBox* sender, bool checked)> CheckedChanged = NULL;
		CheckBox();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual void SetCheck(bool checked);
		virtual bool GetCheck();
		virtual~CheckBox();
	};

};