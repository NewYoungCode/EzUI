#pragma once
#include "Label.h"
namespace EzUI {
	class UI_EXPORT Button :
		public Label
	{
		UI_GetClassName()
	public:
		Button();
		virtual ~Button();
	};
};