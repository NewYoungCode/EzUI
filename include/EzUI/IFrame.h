#pragma once
#include "EzUI.h"
#include "UILoader.h"

namespace ezui {
	//内联页面 内部控件与外部隔离
	class UI_EXPORT IFrame :public Control {
	private:
		UILoader m_umg;//内部UI管理器
	private:
		virtual Control* AddChild(Control* childCtl)override final;
		virtual void Remove(Control* childCtl, bool freeCtrl = false)override final;
	public:
		//对外暴露消息通知回调
		std::function<void(Control*, EventArgs&)> NotifyHandler = NULL;
		IFrame(Object* parentObject = NULL);
		virtual ~IFrame();
		//是否为Frame
		virtual bool IsFrame()override final;
		//从文件中加载xml
		void LoadXml(const UIString& fileName);
		//设置唯一布局
		void SetLayout(Control* ctrl);
		//获取布局
		Control* GetLayout();
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;
		//消息通知
		virtual void OnNotify(Control* sender, EventArgs& args);
		//获取UI管理器
		UILoader* GetUILoader();
	};
};