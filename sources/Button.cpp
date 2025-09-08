#include "Button.h"

namespace ezui {
	void Button::Init()
	{
		Style.Cursor = LoadCursor(Cursor::HAND);
	}
	Button::Button(Object* ownerObject):Label(ownerObject)
	{
		Init();
	}
	Button::~Button()
	{
	}
};