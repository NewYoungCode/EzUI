#pragma once
#include "EzUI.h"
#include "UIManager.h"

namespace ezui {
	//内联页面
	class UI_EXPORT IFrame :public Control {
	private:
		UIManager _umg;
	public:
		IFrame();
		virtual ~IFrame();
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;
		//获取UI管理器
		UIManager* GetUIManager();
	};
};