#include "RadioButton.h"
namespace EzUI {
	RadioButton::RadioButton()
	{
	}
	RadioButton::RadioButton(Control* parent) :CheckBox(parent)
	{
	}
	void RadioButton::OnMouseClick(MouseButton btn, const Point& pt)
	{
		__super::OnMouseClick(btn, pt);
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