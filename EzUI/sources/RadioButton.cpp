#include "RadioButton.h"

namespace EzUI {
	RadioButton::RadioButton()
	{
	}
	RadioButton::RadioButton(Control* parent) :CheckBox(parent)
	{
	}
	void RadioButton::SetCheck(bool checked)
	{
		__super::SetCheck(checked);
		if (checked == true) {
			for (auto& it : Parent->GetControls()) {
				RadioButton* rbtn = dynamic_cast<RadioButton*>(it);
				if (rbtn && rbtn != this && rbtn->GetCheck() == true) {
					rbtn->SetCheck(false);
				}
			}
		}
	}
	RadioButton::~RadioButton()
	{
	}
};