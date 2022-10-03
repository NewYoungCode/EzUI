#pragma once
#include "Layout.h"

namespace EzUI {
	class UI_EXPORT  HLayout :
		public Layout
	{
	public:
		bool OnSize(const Size& size)override;
		void ResumeLayout()override;
	};
	using HBox = HLayout;
};