#include "RadioButton.h"
namespace ezui {
	RadioButton::RadioButton()
	{
	}
	void RadioButton::OnMouseClick(const MouseEventArgs& arg)
	{
		__super::OnMouseClick(arg);
		SetCheck(true);
		if (GetCheck() == true) {
			for (auto& it : Parent->GetControls()) {
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