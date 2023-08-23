#include "CheckBox.h"

namespace EzUI {
	CheckBox::CheckBox()
	{
	}
	CheckBox::~CheckBox()
	{
	}
	void CheckBox::SetCheck(bool checked)
	{
		_checked = checked;
	}
	bool CheckBox::GetCheck()
	{
		return _checked;
	}
	void CheckBox::OnDpiChange(const DpiChangeEventArgs& args) {
		if (args.Scale != this->GetScale()) {
			this->CheckedStyle.Scale(args.Scale / this->GetScale());
		}
		__super::OnDpiChange(args);
	}
	void CheckBox::SetAttribute(const EString& key, const EString& value)
	{
		__super::SetAttribute(key, value);
		if (key == "checked") {
			if (value == "true") {
				SetCheck(true);
				return;
			}
			if (value == "false") {
				SetCheck(false);
				return;
			}
		}
	}
	ControlStyle& CheckBox::GetDefaultStyle()
	{
		if (GetCheck()) {
			return this->CheckedStyle;
		}
		return __super::GetDefaultStyle();
	}
	ControlStyle& CheckBox::GetStyle(const ControlState& _state) {
		if (_state == ControlState::Static) {
			if (GetCheck()) {
				return this->CheckedStyle;
			}
		}
		return __super::GetStyle(_state);
	}

	void CheckBox::OnMouseClick(const MouseEventArgs& arg) {
		__super::OnMouseClick(arg);
		SetCheck(!GetCheck());
	}
};