#include "RadioButton.h"
namespace ezui {
	RadioButton::RadioButton(Object* parentObject):CheckBox(parentObject)
	{
	}
	void RadioButton::OnMouseDown(const MouseEventArgs& arg)
	{
		__super::OnMouseDown(arg);
		SetCheck(true);
		if (GetCheck() == true) {
			for (auto& it : GetParent()->GetControls()) {
				RadioButton* rbtn = dynamic_cast<RadioButton*>(it);
				if (rbtn && rbtn != this && rbtn->GetCheck() == true) {
					rbtn->SetCheck(false);
					rbtn->Invalidate();
				}
			}
		}
	}
	RadioButton::~RadioButton()
	{
	}
};