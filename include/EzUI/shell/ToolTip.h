#pragma once
#include "control/Label.h"
#include "window/BorderlessWindow.h"
namespace ezui {
	/// 提供工具提示（ToolTip)
	class EZUI_API ToolTip : public BorderlessWindow
	{
		Label m_label;
		Animation* m_ani;
	protected:
		//当前显示提示发生的事件
		virtual void OnShowTooltip(Control* ownerCtrl, const UIString& text);

		//隐藏提示发生的事件
		virtual void OnHideTooltip();
	public:
		ToolTip(Object* owner = NULL);

		//设置提示文本
		void SetText(Control* ownerCtrl, const UIString& text);

		//显示文本提示
		void ShowTooltip(Control* ownerCtrl, const UIString& text);

		//关闭文本提示
		void HideTooltip();

		virtual ~ToolTip();
	};
};