#include "CheckBox.h"

namespace EzUI {
	CheckBox::CheckBox()
	{
	}
	CheckBox::CheckBox(Control* parent) :Label(parent)
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

	void CheckBox::OnMouseClick(MouseButton btn, const Point& pt) {
		__super::OnMouseClick(btn, pt);
		SetCheck(!GetCheck());
	}
};