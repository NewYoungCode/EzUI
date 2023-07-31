#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT  VLayout :
		public Control
	{
	public:
		VLayout();
		virtual ~VLayout();
	public:
		void OnLayout()override;
	};
	using VBox = VLayout;
};