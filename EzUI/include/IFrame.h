#pragma once
#include "VLayout.h"
namespace EzUI {
	class IFrame :public VLayout {
	public:
		IFrame();
		IFrame(Control*);
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual ~IFrame();
	};
};