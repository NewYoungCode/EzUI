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
		UILoader* m_loader;//内部UI管理器
	private:
		//当前 Frame 的唯一通知处理器
		EventHandler m_notifyHandler;
		//未设置通知处理器时执行的默认通知逻辑
		void DefaultNotify(Control* sender, EventArgs* args);
	protected:
		/// 接收当前 Frame 范围内的控件通知。
		/// 设置了通知处理器时由处理器完全接管，否则执行默认通知逻辑。
		virtual void OnNotify(Control* sender, EventArgs* args);
	public:
		Frame(Object* ownerObject = NULL);

		//是否为Frame
		virtual bool IsFrame()const EZUI_OVERRIDE EZUI_FINAL;

		/// 设置当前 Frame 的唯一通知处理器。
		/// Frame 内部控件的通知会发送到距离该控件最近的所属 Frame。
		/// 重复调用会替换已有处理器；传入空 EventHandler 会清除处理器并恢复默认通知逻辑。
		/// 设置非空处理器后，通知由该处理器完全接管，不再自动执行默认通知逻辑。
		void SetNotifyHandler(const EventHandler& handler);

		/// 手动向当前 Frame 发送通知，使用与内部控件通知相同的分发规则。
		void SendNotify(Control* sender, EventArgs* args);

		//从文件中加载xml
		void LoadXml(const UIString& fileName);

		/// 设置唯一布局；takeOwnership 为 true 时 Frame 同时取得布局控件的所有权。
		void SetLayout(Control* ctrl, bool takeOwnership = false);

		//获取布局
		Control* GetLayout();

		// Supported attributes:
		// src: xml file path
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)EZUI_OVERRIDE;

		//获取UI管理器
		UILoader* GetUILoader();

		virtual ~Frame();
	};
};
