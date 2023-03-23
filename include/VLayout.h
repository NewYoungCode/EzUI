#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT  VLayout :
		public Control
	{
		UI_GetClassName()
	public:
		virtual ~VLayout();
	public:
		void OnSize(const Size& size)override;
		void OnLayout()override;
	};
	using VBox = VLayout;
};