#include "UISelector.h"

namespace EzUI {
	UISelector::UISelector(const std::vector<Control*>& controls)
	{
		this->ctls = controls;
	}
	UISelector::UISelector(const std::list<Control*>& controls)
	{
		this->ctls.resize(controls.size());
		for (auto& it : controls) {
			this->ctls.push_back(it);
		}
	}
	UISelector::UISelector(Control* ctl)
	{
		this->ctl = ctl;
	}
	UISelector::UISelector(Control* ct, const UIString& _mathStr)
	{
		UIString mathStr = _mathStr;
		mathStr.replace("  ", " ");
		//模仿jquery进行元素匹配
	}
	UISelector::~UISelector()
	{
	}
	UISelector& UISelector::Css(const UIString& styleStr)
	{
		/*	for (auto& it : this->ctls) {
				if (notCtl == it)continue;
				it->SetStyleSheet(styleStr);
			}
			if (ctl) {
				if (notCtl == ctl)return *this;
				ctl->SetStyleSheet(styleStr);
			}*/
		return *this;
	}
	UISelector& UISelector::CssHover(const UIString& styleStr)
	{
		/*	for (auto& it : this->ctls) {
				if (notCtl == it)continue;
				it->SetStyleSheet(styleStr, ControlState::Hover);
			}
			if (ctl) {
				if (notCtl == ctl)return *this;
				ctl->SetStyleSheet(styleStr, ControlState::Hover);
			}*/
		return *this;
	}
	UISelector& UISelector::CssActive(const UIString& styleStr)
	{
		/*	for (auto& it : this->ctls) {
				if (notCtl == it)continue;
				it->SetStyleSheet(styleStr, ControlState::Active);
			}
			if (ctl) {
				if (notCtl == ctl)return *this;
				ctl->SetStyleSheet(styleStr, ControlState::Active);
			}*/
		return *this;
	}
	UISelector& UISelector::Attr(const UIString& key, const UIString& value)
	{
		for (auto& it : this->ctls) {
			if (notCtl == it)continue;
			it->SetAttribute(key, value);
		}
		if (ctl) {
			if (notCtl == ctl)return *this;
			ctl->SetAttribute(key, value);
		}
		return *this;
	}
	UISelector& UISelector::Refresh()
	{
		for (auto& it : this->ctls) {
			it->Invalidate();
		}
		if (ctl) {
			ctl->Invalidate();
		}
		return *this;
	}
	UISelector& UISelector::Not(Control* fiterCtl) {

		this->notCtl = fiterCtl;
		return *this;
	}
};