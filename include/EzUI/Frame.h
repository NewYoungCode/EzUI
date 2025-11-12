#pragma once
#include "EzUI.h"
#include "UILoader.h"

namespace ezui {
	/// 内联页面，隔离内部控件与外部的ID、类名和样式
	/// 避免命名冲突，提供独立的样式作用域（类似前端iframe）
	class UI_EXPORT Frame :public Control {
		friend class Control;
		friend class Window;
	private:
		UILoader& m_loader;//内部UI管理器
	private:
		//默认处理函数
		void DefaultNotify(Control* sender, EventArgs& args);
	protected:
		//消息通知
		virtual void OnNotify(Control* sender, EventArgs& args);
	public:
		//对外暴露消息通知回调
		std::function<void(Control*, EventArgs&)> NotifyHandler = NULL;
		Frame(Object* ownerObject = NULL);
		virtual ~Frame();
		//是否为Frame
		virtual bool IsFrame()override final;
		//手动发送通知
		void SendNotify(Control* sender, EventArgs& args);
		//从文件中加载xml
		void LoadXml(const UIString& fileName);
		//设置唯一布局
		void SetLayout(Control* ctrl);
		//获取布局
		Control* GetLayout();
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;
		//获取UI管理器
		UILoader* GetUILoader();
	};
};