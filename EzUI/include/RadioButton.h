#pragma once
#include "CheckBox.h"
namespace EzUI {
	class UI_EXPORT RadioButton :
		public CheckBox
	{
	public:
		RadioButton();
		RadioButton(Control* parent);
		virtual void SetCheck(bool checked)override;
		virtual~RadioButton();
	};
};