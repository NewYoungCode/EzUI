#pragma once
#include "Layout.h"

namespace EzUI {

	class UI_EXPORT  VLayout :
		public Layout
	{
	public:
		virtual ~VLayout();
	public:
		bool OnSize(const Size& size)override;
		void OnLayout()override;
	};
	using VBox = VLayout;
};