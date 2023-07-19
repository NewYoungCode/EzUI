#pragma once
#include "CheckBox.h"
namespace EzUI {
	class UI_EXPORT RadioButton :
		public CheckBox
	{
	protected:
		virtual void OnMouseClick(MouseButton btn, const Point& pt)override;
	public:
		RadioButton();
		RadioButton(Control* parent);
		virtual~RadioButton();
	};
};