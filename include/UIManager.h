#pragma once
#include "EzUI.h"
#include "Control.h"

namespace UIManager {
	extern std::map<EString, EString> styles;//Ĭ����ʽ����
	extern std::map<EString, EString> styles_active;//������ʽ����
	extern  std::map<EString, EString> styles_hover;//���������ʽ����
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
}
