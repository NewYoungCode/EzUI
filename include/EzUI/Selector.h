#pragma once
#include "UIManager.h"
namespace EzUI {
	//选择器(多功能选择器暂时未完善)
	class UI_EXPORT Selector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		std::vector<Control*> ctls;
		Selector& NextName(const EString& key) { return *this; };
		Selector& NextId(const EString& key) { return *this; };
	public:
		Selector(const std::vector<Control*>& controls);
		Selector(const std::list<Control*>& controls);
		Selector(Control* control);
		Selector(Control* control, const EString& mathStr);
		virtual ~Selector();
		Selector& Css(const EString& styleStr);
		Selector& CssHover(const EString& styleStr);
		Selector& CssActive(const EString& styleStr);
		Selector& Attr(const EString& key, const EString& value);
		Selector& Refresh();
		Selector& Not(Control* fiterCtl);
	};
#define $ Selector
};