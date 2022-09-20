#pragma once
#include "Control.h"
namespace EzUI {

	class UI_EXPORT  Layout :
		public Control
	{
	public:
		Layout();
		virtual void ResumeLayout();
		virtual ~Layout();
	};
	using Box = Layout;
};