#pragma once
#include "EzUI.h"
#include "Control.h"
#include "Window.h"
namespace EzUI {


	namespace EzUIManager {
		std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);
		Layout* UI_EXPORT LoadLayout(const EString& filename);
		void  UI_EXPORT LoadControl(const EString& xmlRaw, std::vector<Control*>& controls);
		extern std::map<EString, EString> styles;//默认样式集合
		extern std::map<EString, EString> styles_active;//按下样式集合
		extern std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
	}
	struct _Selector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		Controls ctls;
		_Selector& NextName(const EString& key) {};
		_Selector& NextId(const EString& key) {};
	public:
		_Selector(const Controls& Controls);
		_Selector(Control* control);
		_Selector& Css(const EString& styleStr);
		_Selector& Attr(const EString& key, const EString& value);
		_Selector& Refresh();
		_Selector& Not(Control* fiterCtl);
	};
#define $ _Selector
};