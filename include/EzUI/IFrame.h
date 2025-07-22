#pragma once
#include "EzUI.h"
#include "UIManager.h"

namespace ezui {
	//内联页面
	class UI_EXPORT IFrame :public Control {
	private:
		UIManager m_umg;
	public:
		IFrame(Object* parentObject = NULL);
		virtual ~IFrame();
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;
		virtual bool OnNotify(Control* sender, EventArgs& args);
		//获取UI管理器
		UIManager* GetUIManager();
	};
};