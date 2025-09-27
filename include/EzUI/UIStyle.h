#pragma once
#include "EzUI.h"
namespace ezui {
	struct Style
	{
		ControlState m_styleType;
		UIString m_selectorName;
		UIString m_styleStr;
	};
	extern UI_EXPORT void AnalysisStyle(const UIString& styleStr, std::list<Style>* out);//分析样式
	extern UI_EXPORT int MathStyle(Control* ctl, const UIString& selectorName, const std::list<Style>& selectors);
	extern UI_EXPORT void ApplyStyle(Control* ctl, const UIString& selectName, const std::list<Style>& selectors);
	extern UI_EXPORT void ApplyStyle(Control* ctl, const std::list<Style>& selectors);
};