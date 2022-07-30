#pragma once
#include "EzUI.h"
#include "Control.h"
#include "Window.h"
namespace UIManager {
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
	void  UI_EXPORT LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
	void AppendControl(const EString& xmlContent, Window* wind);//暂时没用到...预留吧
	extern std::map<EString, EString> styles;//默认样式集合
	extern std::map<EString, EString> styles_active;//按下样式集合
	extern std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
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