#pragma once
#include "Label.h"

namespace EzUI {

	class UI_EXPORT CheckBox :
		public Label,public ISelect
	{
	protected:
		virtual ControlStyle& GetStyle(const ControlState& _state)override;
		virtual void OnMouseClick(MouseButton btn, const Point& pt)override;	
	public:
		CheckBox();
		CheckBox(Control* parent);
		virtual~CheckBox();
		void SetAttribute(const EString& key, const EString& value)override;
	};

};