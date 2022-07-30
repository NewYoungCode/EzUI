#pragma once
#include "EzUI.h"
#include "Control.h"
#include "Window.h"
namespace UIManager {
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
	void  UI_EXPORT LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
	void AppendControl(const EString& xmlContent, Window* wind);//��ʱû�õ�...Ԥ����
	extern std::map<EString, EString> styles;//Ĭ����ʽ����
	extern std::map<EString, EString> styles_active;//������ʽ����
	extern std::map<EString, EString> styles_hover;//���������ʽ����
}
struct _Selector
{
private:
	Controls ctls;
public:
	_Selector(const Window*wind,const EString& str);
	_Selector(const Controls& Controls);
	size_t css(const EString& styleStr);
	size_t attr(const EString& key, const EString& value);
};
#define $ _Selector