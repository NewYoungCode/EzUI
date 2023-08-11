#include "BorderlessWindow.h"
namespace EzUI {
	BorderlessWindow::BorderlessWindow(int width, int height, HWND owner) : Window(width, height, owner, /* WS_THICKFRAME |*/ WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, NULL)
	{
		_boxShadow = new ShadowWindow(width, height, Hwnd());
		UpdateShadow();
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadow();
	}
	ShadowWindow* BorderlessWindow::GetShadowWindow()
	{
		return _boxShadow;
	}
	void BorderlessWindow::SetShadow(int padding)
	{
		_shadowWidth = padding;
		UpdateShadow();
	}
	void BorderlessWindow::OnRect(const Rect& rect) {
		__super::OnRect(rect);
		if (!_firstPaint) {
			//避免先显示阴影再显示窗口的问题
			UpdateShadow();
		}
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
			/*	case WM_NCPAINT:
				{
					return 0;
				}*/
		//case WM_NCCALCSIZE:
		//{
		//	return 0;
		//}
		//case WM_NCACTIVATE:
		//{
		//	return 0;
		//}
		case WM_NCHITTEST: {
			if (!::IsZoomed(Hwnd()) && Zoom) {
				return ZoomWindow(lParam);
			}
			break;
		}
		default:
			break;
		}
		LRESULT ret = __super::WndProc(uMsg, wParam, lParam);
		if (_firstPaint && uMsg == WM_PAINT) {
			_firstPaint = false;
			UpdateShadow();
		}
		return ret;
	}
}