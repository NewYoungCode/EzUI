#include "Button.h"

namespace EzUI {
	Button::Button() {
		Style.ForeColor = Color::Black;
		Style.BackgroundColor = { 170,200,200,200 };//the slider color
		HoverStyle.BackgroundColor = { 200,200,200,200 };//the slider color
		ActiveStyle.BackgroundColor = { 127,200,200,200 };//the bar backgroundcolor
		this->Cursor = Cursor::HAND;
	}
	Button::~Button()
	{

	}
};