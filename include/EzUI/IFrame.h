#pragma once
#include "EzUI.h"
#include "UIManager.h"

namespace ezui {
	//内联页面 内部控件与外部隔离
	class UI_EXPORT IFrame :public Control {
	private:
		UIManager m_umg;//内部UI管理器
	private:
		virtual Control* Add(Control* childCtl)override;
		virtual void Remove(Control* childCtl, bool freeCtrl = false)override;
	public:
		//对外暴露消息通知回调
		std::function<void(Control*, EventArgs&)> NotifyHandler = NULL;
		IFrame(Object* parentObject = NULL);
		virtual ~IFrame();
		//从文件中加载xml
		void LoadXml(const UIString& fileName);
		//从内存中加载xml
		void LoadXml(const char* fileData, size_t fileSize);
		//设置唯一布局
		void SetLayout(Control* ctrl);
		//获取布局
		Control* GetLayout();
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;
		//消息通知
		virtual void OnNotify(Control* sender, EventArgs& args);
		//获取UI管理器
		UIManager* GetUIManager();
	};
};