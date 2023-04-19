#include "HostWnd.h"

namespace EzUI {

	HostWnd::HostWnd(Control* parent) :Control(parent)
	{
	}
	HostWnd::HostWnd(HWND hwnd) {
		this->_hWnd = hwnd;
	}
	void HostWnd::OnLoad() {
		SetParent(_hWnd, this->PublicData->HANDLE);
		::ShowWindow(_hWnd,SW_SHOW);
	}
	HostWnd::~HostWnd() {

	}
	void HostWnd::OnPaint(PaintEventArgs& args) {

	}
	void HostWnd::OnSize(const Size& sz)
	{
		if (_hWnd) {
			auto rect = GetClientRect();
			::MoveWindow(_hWnd, rect.X, rect.Y, rect.Width, rect.Height, TRUE);
		}
	}

}