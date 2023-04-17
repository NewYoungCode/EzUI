#pragma once
#include "Label.h"
namespace EzUI {
	class UI_EXPORT Button :
		public Label
	{
	private:
		void Init();
	public:
		Button();
		Button(Control* parent);
		virtual ~Button();
	};
};