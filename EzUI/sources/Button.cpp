#include "Button.h"

namespace EzUI {
	void Button::Init()
	{
		Style.ForeColor = Color::Black;
		Style.BackgroundColor = { 253,253,253 };//the slider color
		Style.Radius = 5;
		HoverStyle.BackgroundColor = { 224,238,249 };//the slider color
		HoverStyle.SetBorder(Color(0, 120, 212), 2);
		ActiveStyle.SetBorder(Color(0, 120, 212), 2);
		this->Cursor = Cursor::HAND;
	}
	Button::Button() {
		Init();
	}
	Button::Button(Control* parent):Label(parent)
	{
		Init();
	}
	Button::~Button()
	{

	}
};