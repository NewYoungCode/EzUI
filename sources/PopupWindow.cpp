#include "PopupWindow.h"
namespace EzUI {
	PopupWindow::PopupWindow(int_t width, int_t height, HWND owner) :LayeredWindow(width, height, owner)
	{
		this->SetResizable(false);
	}
	PopupWindow::PopupWindow(int_t width, int_t height, Control* ownerCtl) :LayeredWindow(width, height, ownerCtl->PublicData->HANDLE)
	{
		this->_ownerCtl = ownerCtl;
		this->SetResizable(false);
	}
	void PopupWindow::Show()
	{
		int_t x, y, width, height;
		const Rect& rect = this->GetClientRect();
		Rect ctlRect;

		POINT location;
		if (_ownerCtl) {
			ctlRect = _ownerCtl->GetClientRect();
			location.x = ctlRect.GetLeft();
			location.y = ctlRect.GetBottom();
			::ClientToScreen(_ownerCtl->PublicData->HANDLE, &location);
		}
		else {
			::GetCursorPos(&location);
		}

		MonitorInfo monitorInfo;
		GetMontior(&monitorInfo, ::GetWindowOwner(Hwnd()));
		x = location.x;
		y = location.y;
		width = rect.Width;
		height = rect.Height;
		if (_ownerCtl) {
			width = ctlRect.Width;
		}
		if ((location.y + height) > monitorInfo.Rect.GetBottom()) {
			y -= height;
			if (_ownerCtl) {
				y -= ctlRect.Height;
			}
		}
		if ((location.x + width) > monitorInfo.Rect.GetRight()) {
			x -= width;
		}
		::SetWindowPos(Hwnd(), NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
		__super::Show();
		::SetForegroundWindow(Hwnd());
		this->SetTopMost(true);
	}

	int_t PopupWindow::ShowModal(bool disableOnwer)
	{
		return __super::ShowModal(disableOnwer);
	}

	PopupWindow::~PopupWindow()
	{
	}

	void PopupWindow::OnKillFocus(HWND hWnd)
	{
		::DestroyWindow(Hwnd());
	}
};