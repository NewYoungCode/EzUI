#include "Form.h"

namespace EzUI {
	MenuWindow::MenuWindow(int cx, int cy, HWND owner) :BorderlessWindow(cx, cy, owner)
	{
	}
	LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_NCPAINT)
		{
			HWND hWnd = _hWnd;
			HDC hdc;
			hdc = GetWindowDC(hWnd);
			// Paint into this DC
			RECT rcWin;
			GetWindowRect(hWnd, &rcWin);
			OffsetRect(&rcWin, -rcWin.left, -rcWin.top);
			auto b = CreateSolidBrush(RGB(255, 0, 0));

			for (int i = 0; i < 4; i++)
			{
				FrameRect(hdc, &rcWin, b);
				InflateRect(&rcWin, -1, -1);
			}
			ReleaseDC(hWnd, hdc);
			return 0;
		}

		if (uMsg == WM_NCHITTEST) {
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
		}
		if (uMsg == WM_KILLFOCUS) {
			HWND wnd = (HWND)wParam;
			if (wnd != this->_boxShadow->_hWnd) {
				this->Close();
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void MenuWindow::Show(int cmdShow)
	{
		HWND Owner = ::GetWindowOwner(_hWnd);
		RECT OwnerRect;
		::GetWindowRect(Owner, &OwnerRect);
		auto rect = GetRect();

		POINT mousePos;
		GetCursorPos(&mousePos);
		POINT clientPos = mousePos;
		ScreenToClient(Owner, &clientPos);
		Debug::Log("%d %d", clientPos.x, clientPos.y);

		int height = OwnerRect.bottom - OwnerRect.top;
		//if (rect.Height>height  ) {
		::MoveWindow(_hWnd, mousePos.x, mousePos.y, rect.Width, rect.Height, FALSE);
		/*}
		else {
			::MoveWindow(_hWnd, mousePos.x, mousePos.y- rect.Height, rect.Width, rect.Height, FALSE);
		}*/
		__super::Show(cmdShow);
		::SetForegroundWindow(_hWnd);
	}
};