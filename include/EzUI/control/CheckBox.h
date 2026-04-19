#pragma once
#include "Label.h"

namespace ezui {
	//复选框
	class EZUI_API CheckBox :
		public Label
	{
	public:
		//选中样式
		StyleProxy CheckedStyle;
		//选中状态发送变化的回调函数
		std::function<void(CheckBox* sender, bool checked)> CheckedChanged = NULL;
	protected:
		virtual StyleProxy& GetStyle(VisualState _state)override;
		virtual void OnMouseDown(MouseEventArgs* arg)override;
		virtual void OnDpiChanged(DpiChangedEventArgs* args)override;
	public:
		CheckBox(Object* ownerObject = NULL);

		// Supported attributes:
		// checked: true | false
		// All Label attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value)override;

		//设置选中状态
		virtual void SetCheck(bool checked, bool triggerChangedEvent = false);

		//获取选中状态
		virtual bool GetCheck();

		virtual ~CheckBox();
	};

};