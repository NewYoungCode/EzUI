#pragma once
#include "EzUI.h"
#include "Control.h"

namespace UIManager {
	extern std::map<EString, EString> styles;//Ĭ����ʽ����
	extern std::map<EString, EString> styles_active;//������ʽ����
	extern  std::map<EString, EString> styles_hover;//���������ʽ����
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
	void  UI_EXPORT LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
	void AppendControl(const EString& xmlContent, Window* wind);//��ʱû�õ�...Ԥ����
}
