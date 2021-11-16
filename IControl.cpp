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