#include "ToolWindow.h"
namespace EzUI {
	MenuWindow::MenuWindow(int width, int height, HWND owner) :LayeredWindow(width, height, owner)
	{
		this->Zoom = false;
	}
	MenuWindow::MenuWindow(int width, int height, Control* ownerCtl) :LayeredWindow(width, height, ownerCtl->PublicData->HANDLE)
	{
		this->_ownerCtl = ownerCtl;
		this->Zoom = false;
	}
	LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KILLFOCUS) {
			HWND wnd = (HWND)wParam;
			if (this->GetShadowWindow() && wnd != this->GetShadowWindow()->Hwnd()) {
				this->OnKillFocus(wnd);
			}
			else if (this->GetShadowWindow() == NULL) {
				this->OnKillFocus(wnd);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void MenuWindow::Show(int cmdShow)
	{
		int x, y, width, height;
		const Rect& rect = this->GetClientRect();
		Rect ctlRect;
		//获取显示器 达到鼠标在哪个显示器 窗口就在哪个显示器中显示
		std::list<MonitorInfo> outMonitorInfo;
		GetMonitors(&outMonitorInfo);
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

		MonitorInfo* monitorInfo = NULL;
		for (auto& it : outMonitorInfo) {
			if (it.Rect.Contains(location.x, location.y)) {
				monitorInfo = &it;
				break;
			}
		}
		x = location.x;
		y = location.y;
		width = rect.Width;
		height = rect.Height;
		if (_ownerCtl) {
			width = ctlRect.Width;
		}
		if ((location.y + height) > monitorInfo->Rect.Height) {
			y -= height;
			if (_ownerCtl) {
				y -= ctlRect.Height;
			}
		}
		if ((location.x + width) > monitorInfo->Rect.Width) {
			x -= width;
		}
		::SetWindowPos(Hwnd(), HWND_TOPMOST, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
		__super::Show(cmdShow);
		::SetForegroundWindow(Hwnd());
	}

	int MenuWindow::ShowModal()
	{
		HWND OwnerHwnd = ::GetWindowOwner(Hwnd());
		Show();
		MSG msg{ 0 };
		while (::IsWindow(Hwnd()) && ::GetMessage(&msg, NULL, 0, 0) && msg.message != WM_QUIT)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {//
			::PostQuitMessage(msg.wParam);
		}
		::SetFocus(OwnerHwnd);
		return 0;
	}

	void MenuWindow::OnKillFocus(HWND hWnd)
	{
		this->Close();
	}
};