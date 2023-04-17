#include "BorderlessWindow.h"
namespace EzUI {
	BorderlessWindow::BorderlessWindow(int width, int height, HWND owner) : Window(width, height, owner, WS_POPUP | WS_MINIMIZEBOX, NULL)
	{

		_boxShadow = new ShadowWindow(width, height, Hwnd());
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
		__super::OnRect(rect);
		UpdateShadow();
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
			if (!::IsZoomed(Hwnd()) && Zoom) {
				return ZoomWindow(lParam);
			}
			break;
		}
						 /*	case WM_NCLBUTTONUP: {
								 ShowMax();
								 break;
							 }*/
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
}