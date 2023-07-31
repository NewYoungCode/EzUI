#include "Button.h"

namespace EzUI {
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