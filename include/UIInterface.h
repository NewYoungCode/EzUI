#pragma once
#include "UIDef.h"
#include "IType.h"
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  UIInterface 
	{
	public:
		UIInterface(const EString& filename);
		virtual ~UIInterface();
	protected:

	};
};