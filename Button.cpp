#include "Button.h"

Button::Button() {

	Style.ForeColor = Color::Black;
	Style.SetBorder(Color(173, 173, 173), 1);
	Style.BackgroundColor = Color(225, 225, 225);

	HoverStyle.SetBorder(Color(0, 120, 215), 1);
	HoverStyle.BackgroundColor = Color(229, 241, 251);

	ActiveStyle.SetBorder(Color(0, 120, 215), 1);
	ActiveStyle.BackgroundColor = Color(204, 228, 247);



	//this->Cursor = IDC_HAND;
}

Button::~Button()
{

}

