#include "CheckBox.h"

namespace ezui {
	CheckBox::CheckBox(Object* parentObject) :Label(parentObject)
	{
	}
	CheckBox::~CheckBox()
	{
	}
	void CheckBox::SetCheck(bool checked)
	{
		m_checked = checked;
		if (checked) {
			this->State = this->State | ControlState::Checked;
		}
		else {
			this->State = this->State & (~ControlState::Checked);
		}
	}
	bool CheckBox::GetCheck()
	{
		return m_checked;
	}
	void CheckBox::OnDpiChange(const DpiChangeEventArgs& args) {
		if (!ezui::IsFloatEqual(args.Scale, this->GetScale())) {
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

	ControlStyle& CheckBox::GetStyle(const ControlState& _state) {
		if ((_state & ControlState::Checked) == ControlState::Checked) {
			return this->CheckedStyle;
		}
		return __super::GetStyle(_state);
	}

	void CheckBox::OnMouseDown(const MouseEventArgs& arg) {
		__super::OnMouseDown(arg);
		SetCheck(!GetCheck());
		if (CheckedChanged) {
			CheckedChanged(this, GetCheck());
		}
		if (GetCheck()) {
			this->State = this->State | ControlState::Checked;
			this->Invalidate();
		}
	}
	void CheckBox::OnMouseEnter(const MouseEventArgs& args)
	{
		__super::OnMouseEnter(args);
		if (GetCheck()) {
			this->State = this->State | ControlState::Checked;
			this->Invalidate();
		}
	}
	void CheckBox::OnMouseUp(const MouseEventArgs& args)
	{
		__super::OnMouseUp(args);
	}
	void CheckBox::OnMouseLeave(const MouseEventArgs& args)
	{
		__super::OnMouseLeave(args);
		if (GetCheck()) {
			this->State = this->State | ControlState::Checked;
			this->Invalidate();
		}
	}
};