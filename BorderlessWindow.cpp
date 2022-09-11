#include "BorderlessWindow.h"
namespace EzUI {
	LRESULT __ZoomWindow2(const HWND& _hWnd, const  LPARAM& lParam) {
		RECT rc;
		GetWindowRect(_hWnd, &rc);
		POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		int x = 4;//
		if (pt.x < rc.left + x)
		{
			if (pt.y < rc.top + x)return HTTOPLEFT;//
			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//
			return HTLEFT;//
		}
		if (pt.x >= rc.right - x)//
		{
			if (pt.y < rc.top + x)return HTTOPRIGHT;//
			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//
			return HTRIGHT;//
		}
		if (pt.y < rc.top + x)return HTTOP;//
		if (pt.y >= rc.bottom - x)return HTBOTTOM;//
		return HTCLIENT;//ָ
	}
	BorderlessWindow::BorderlessWindow(int width, int height, HWND owner) : Window(width, height, owner, WS_POPUP | WS_MINIMIZEBOX, NULL)
	{
		_boxShadow = new BoxShadow(width, height, _hWnd);
		UpdateShadow();
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadow();
	}
	void BorderlessWindow::SetShadow(int width)
	{
		_shadowWidth = width;
		UpdateShadow();
	}
	void BorderlessWindow::OnRect(const Rect& rect) {
		UpdateShadow();
		__super::OnRect(rect);
	}
	void BorderlessWindow::Hide() {
		__super::Hide();
		UpdateShadow();
	}
	void BorderlessWindow::UpdateShadow() {
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
	}
	void BorderlessWindow::CloseShadow()
	{
		if (_boxShadow) {
			delete _boxShadow;
			_boxShadow = NULL;
		}
	}
	LRESULT  BorderlessWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_NCHITTEST: {
			if (!::IsZoomed(_hWnd) && Zoom) {
				return __ZoomWindow2(_hWnd, lParam);
			}
			break;
		}
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
}