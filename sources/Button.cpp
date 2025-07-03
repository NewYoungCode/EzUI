#include "Button.h"

namespace ezui {
	void Button::Init()
	{
		Style.Cursor = LoadCursor(Cursor::HAND);
	}
	Button::Button() {
		Init();
	}
	Button::~Button()
	{
	}
};