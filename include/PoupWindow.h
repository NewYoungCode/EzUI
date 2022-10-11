#pragma once
#include "IType.h"
#include "BorderlessWindow.h"
namespace EzUI {
	class PoupWindow :public BorderlessWindow {
	public:
		PoupWindow(int cx, int cy, HWND owner) : BorderlessWindow(cx, cy, owner)
		{
			this->SetShadow(5);
		}
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_NCHITTEST) {
				return ::DefWindowProc(Hwnd(), uMsg, wParam, lParam);
			}
			if (uMsg == WM_KILLFOCUS) {
				HWND wnd = (HWND)wParam;
				if (wnd != this->_boxShadow->_hWnd) {
					this->Hide();
				}
			}
			return __super::WndProc(uMsg, wParam, lParam);
		}
		void Show(int cmdShow)
		{
			__super::Show(cmdShow);
			//HWND Owner = ::GetWindowOwner(Hwnd());
			//RECT OwnerRect;
			//::GetWindowRect(Owner, &OwnerRect);
			//auto rect = GetRect();
			// 
			//POINT mousePos;
			//GetCursorPos(&mousePos);
			//POINT clientPos = mousePos;
			//ScreenToClient(Owner, &clientPos);
			////Debug::Log("%d %d", clientPos.x, clientPos.y);

			//int height = OwnerRect.bottom - OwnerRect.top;
			////if (rect.Height>height  ) {
			//::MoveWindow(Hwnd(), mousePos.x, mousePos.y, rect.Width, rect.Height, FALSE);
			///*}
			//else {
			//	::MoveWindow(_hWnd, mousePos.x, mousePos.y- rect.Height, rect.Width, rect.Height, FALSE);
			//}*/
			//__super::Show(cmdShow);
			::SetForegroundWindow(Hwnd());
		}
	};
};