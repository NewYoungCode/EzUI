#pragma once
#include "Label.h"

namespace ezui {
	//按钮控件
	class EZUI_API Button :
		public Label
	{
	private:
		void Init();
	public:
		Button(Object* ownerObject = NULL);
		virtual ~Button();
	};
};