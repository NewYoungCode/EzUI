#pragma once
#include "EzUI.h"
#include "Painter.h"

namespace Style {
	void UI_EXPORT Erase(EString& str, TCHAR _char = ' ');
	Color UI_EXPORT StringToColor(const EString& str);
	Rect UI_EXPORT StringToRect(const EString& str);
};