#pragma once
#include "Label.h"

namespace ezui {
	class UI_EXPORT Button :
		public Label
	{
	private:
		void Init();
	public:
		Button();
		virtual ~Button();
	};
};