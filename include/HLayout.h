#pragma once
#include "Layout.h"

namespace EzUI {
	class UI_EXPORT  HLayout :
		public Layout
	{
	public:
		void OnSize(const Size& size)override;
		void OnLayout()override;
	};
	using HBox = HLayout;
};