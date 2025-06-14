#include "Selector.h"

namespace EzUI {
	Selector::Selector(const std::vector<Control*>& controls)
	{
		this->ctls = controls;
	}
	Selector::Selector(const std::list<Control*>& controls)
	{
		this->ctls.resize(controls.size());
		for (auto& it : controls) {
			this->ctls.push_back(it);
		}
	}
	Selector::Selector(Control* ctl)
	{
		this->ctl = ctl;
	}
	Selector::Selector(Control* ct, const EString& _mathStr)
	{
		EString mathStr = _mathStr;
		mathStr.replace("  ", " ");
		//模仿jquery进行元素匹配
	}
	Selector::~Selector()
	{
	}
	Selector& Selector::Css(const EString& styleStr)
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
	Selector& Selector::CssHover(const EString& styleStr)
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
	Selector& Selector::CssActive(const EString& styleStr)
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
	Selector& Selector::Attr(const EString& key, const EString& value)
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
	Selector& Selector::Refresh()
	{
		for (auto& it : this->ctls) {
			it->Invalidate();
		}
		if (ctl) {
			ctl->Invalidate();
		}
		return *this;
	}
	Selector& Selector::Not(Control* fiterCtl) {

		this->notCtl = fiterCtl;
		return *this;
	}
};