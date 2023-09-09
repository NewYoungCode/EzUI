#include "PopupWindow.h"
namespace EzUI {
	PopupWindow::PopupWindow(int width, int height, HWND owner) :LayeredWindow(width, height, owner)
	{
		this->Zoom = false;
	}
	PopupWindow::PopupWindow(int width, int height, Control* ownerCtl) :LayeredWindow(width, height, ownerCtl->PublicData->HANDLE)
	{
		this->_ownerCtl = ownerCtl;
		this->Zoom = false;
	}
	LRESULT PopupWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KILLFOCUS) {
			HWND wnd = (HWND)wParam;
			if (this->GetShadowBox() && wnd != this->GetShadowBox()->Hwnd()) {
				this->OnKillFocus(wnd);
			}
			else if (this->GetShadowBox() == NULL) {
				this->OnKillFocus(wnd);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void PopupWindow::Show(int cmdShow)
	{
		int x, y, width, height;
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
		::SetWindowPos(Hwnd(), HWND_TOPMOST, x, y, width, height, NULL);
		__super::Show(cmdShow);
		::SetForegroundWindow(Hwnd());
	}

	int PopupWindow::ShowModal(bool disableOnwer)
	{
		return __super::ShowModal(disableOnwer);
	}

	void PopupWindow::OnKillFocus(HWND hWnd)
	{
		::DestroyWindow(Hwnd());
	}
};