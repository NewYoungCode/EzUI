#pragma once
#include "CheckBox.h"
namespace EzUI {
	class UI_EXPORT RadioButton :
		public CheckBox
	{
	protected:
		virtual void OnMouseClick(const MouseEventArgs& arg)override;
	public:
		RadioButton();
		virtual~RadioButton();
	};
};