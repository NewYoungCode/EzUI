#pragma once
#include "Label.h"

namespace EzUI {

	class UI_EXPORT  RadioButton :
		public Label
	{
	protected:
		bool _checked = false;
	public:
		int Indent = 2;//缩进
		RadioButton();
		virtual	~RadioButton();
		void SetCheck(bool checked = true);
		bool GetCheck();
		virtual void OnMouseClick(MouseButton btn, const Point& pt)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
	};

};