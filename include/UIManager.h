#pragma once
#include "Control.h"
#include "Layout.h"
#include "Window.h"
namespace EzUI {

	namespace UIManager {
		extern UI_EXPORT std::vector<Control*>   LoadControls(const EString& filename);
		extern UI_EXPORT Layout* LoadLayout(const EString& _filename);
		extern UI_EXPORT void   LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
		extern  std::map<EString, EString> styles;//默认样式集合
		extern std::map<EString, EString> styles_active;//按下样式集合
		extern std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
	}
	struct UI_EXPORT _Selector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		Controls ctls;
		_Selector& NextName(const EString& key) { return *this; };
		_Selector& NextId(const EString& key) { return *this; };
	public:
		_Selector(const Controls& Controls);
		_Selector(Control* control);
		_Selector& Css(const EString& styleStr);
		_Selector& CssHover(const EString& styleStr);
		_Selector& CssActive(const EString& styleStr);
		_Selector& Attr(const EString& key, const EString& value);
		_Selector& Refresh();
		_Selector& Not(Control* fiterCtl);
	};
#define $ _Selector
};