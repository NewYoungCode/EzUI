#include "UISelector.h"

namespace ezui {
	UISelector::UISelector(const std::vector<Control*>& controls)
	{
		this->m_ctls = controls;
	}
	UISelector::UISelector(const std::list<Control*>& controls)
	{
		this->m_ctls.resize(controls.size());
		for (auto& it : controls) {
			this->m_ctls.push_back(it);
		}
	}
	UISelector::UISelector(Control* ctl)
	{
		this->m_ctl = ctl;
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
		for (auto& it : this->m_ctls) {
			if (m_notCtl == it)continue;
			it->SetAttribute(key, value);
		}
		if (m_ctl) {
			if (m_notCtl == m_ctl)return *this;
			m_ctl->SetAttribute(key, value);
		}
		return *this;
	}
	UISelector& UISelector::Refresh()
	{
		for (auto& it : this->m_ctls) {
			it->Invalidate();
		}
		if (m_ctl) {
			m_ctl->Invalidate();
		}
		return *this;
	}
	UISelector& UISelector::Not(Control* fiterCtl) {

		this->m_notCtl = fiterCtl;
		return *this;
	}
};