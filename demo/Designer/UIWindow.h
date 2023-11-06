#pragma once
#include "stdafx.h"
namespace EzUI {
	class UIWindow :
		public Window
	{
		virtual bool OnNotify(Control* sender, EventArgs& arg);
	public:
		UIWindow(int w, int h);
		Control* InsertControl(Control* ctl);
	};
};

