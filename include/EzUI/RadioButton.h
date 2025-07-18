#pragma once
#include "CheckBox.h"

namespace ezui {
	class UI_EXPORT RadioButton :
		public CheckBox
	{
	protected:
		virtual void OnMouseClick(const MouseEventArgs& arg)override;
	public:
		RadioButton(Object* parentObject = NULL);
		virtual~RadioButton();
	};
};