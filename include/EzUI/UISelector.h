#pragma once
#include "UIManager.h"

namespace ezui {
	//控件选择器(多功能选择器暂时未完善)
	class UI_EXPORT UISelector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		std::vector<Control*> ctls;
		UISelector& NextName(const UIString& key) { return *this; };
		UISelector& NextId(const UIString& key) { return *this; };
	public:
		UISelector(const std::vector<Control*>& controls);
		UISelector(const std::list<Control*>& controls);
		UISelector(Control* control);
		UISelector(Control* control, const UIString& mathStr);
		virtual ~UISelector();
		UISelector& Css(const UIString& styleStr);
		UISelector& CssHover(const UIString& styleStr);
		UISelector& CssActive(const UIString& styleStr);
		UISelector& Attr(const UIString& key, const UIString& value);
		UISelector& Refresh();
		UISelector& Not(Control* fiterCtl);
	};
#define $ UISelector
};