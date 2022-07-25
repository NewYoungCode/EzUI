#pragma once
#include "EzUI.h"
#include "Control.h"

namespace UIManager {
	extern std::map<EString, EString> styles;//默认样式集合
	extern std::map<EString, EString> styles_active;//按下样式集合
	extern  std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
	void  UI_EXPORT LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
	void AppendControl(const EString& xmlContent, Window* wind);//暂时没用到...预留吧
}
