#include "CheckBox.h"

namespace ezui {
	CheckBox::CheckBox()
	{
	}
	CheckBox::~CheckBox()
	{
	}
	void CheckBox::SetCheck(bool checked)
	{
		m_checked = checked;
	}
	bool CheckBox::GetCheck()
	{
		return m_checked;
	}
	void CheckBox::OnDpiChange(const DpiChangeEventArgs& args) {
		if (args.Scale != this->GetScale()) {
			this->CheckedStyle.Scale(args.Scale / this->GetScale());
		}
		__super::OnDpiChange(args);
	}
	void CheckBox::SetAttribute(const UIString& key, const UIString& value)
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
		if (CheckedChanged) {
			CheckedChanged(this, GetCheck());
		}
	}
};