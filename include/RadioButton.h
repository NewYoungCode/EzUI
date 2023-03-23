#pragma once
#include "Label.h"

namespace EzUI {

	class UI_EXPORT  RadioButton :
		public Label
	{
		UI_GetClassName()
	protected:
		bool _checked = false;
	public:
		int Indent = 2;//Ëõ½ø
		RadioButton();
		virtual	~RadioButton();
		void SetCheck(bool checked = true);
		bool GetCheck();
		virtual void OnMouseClick(MouseButton btn, const Point& pt)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
	};

};