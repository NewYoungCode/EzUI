#pragma once
#include "core/UICore.h"
#include "core/UILoader.h"

namespace ezui {
	/// 内联页面，隔离内部控件与外部的ID、类名和样式
	/// 避免命名冲突，提供独立的样式作用域（类似前端iframe）
	class EZUI_API Frame :public Control {
		friend class Control;
		friend class Window;
	private:
		UILoader* m_loader = NULL;//内部UI管理器
	private:
		//消息通知回调
		EventHandler m_notifyHandler;
		//默认处理函数
		void DefaultNotify(Control* sender, EventArgs* args);
	protected:
		//消息通知
		virtual void OnNotify(Control* sender, EventArgs* args);
	public:
		Frame(Object* ownerObject = NULL);

		//是否为Frame
		virtual bool IsFrame()const override final;

		//设置通知回调处理函数
		void SetNotifyHandler(const EventHandler& handler);

		//手动发送通知
		void SendNotify(Control* sender, EventArgs* args);

		//从文件中加载xml
		void LoadXml(const UIString& fileName);

		//设置唯一布局
		void SetLayout(Control* ctrl, bool autoDelete = false);

		//获取布局
		Control* GetLayout();

		// Supported attributes:
		// src: xml file path
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override;

		//获取UI管理器
		UILoader* GetUILoader();

		virtual ~Frame();
	};
};