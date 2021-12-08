#include "IControl.h"

IControl::IControl(){}
IControl::~IControl(){}
void IControl::OnTimer(){}
UINT_PTR IControl::SetTimer(size_t interval)
{
	if (_hasTimer) {
		return (UINT_PTR)this;
	}
	_hasTimer = true;
	return ::SetTimer(_hWnd, (UINT_PTR)this, interval, NULL);
}
void IControl::KillTimer() {
	::KillTimer(_hWnd, (UINT_PTR)this);
	_hasTimer = false;
}
void IControl::SetAttribute(const EString& attrName, const EString& attrValue) {
	AttributeIterator itor = _attrs.find(attrName);
	if (itor != _attrs.end()) {
		(*itor).second = attrValue;
	}
	else {
		_attrs.insert(std::pair<EString, EString>(attrName, attrValue));
	}
}
const EString IControl::GetAttribute(const EString& attrName) {
	AttributeIterator itor = _attrs.find(attrName);
	if (itor != _attrs.end()) {
		return (*itor).second;
	}
	return "";
}