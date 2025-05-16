#include "BorderlessWindow.h"
namespace EzUI {
	BorderlessWindow::BorderlessWindow(int width, int height, HWND owner, DWORD exStyle) : Window(width, height, owner, /*WS_THICKFRAME |*/ WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, exStyle)
	{
		_shadowBox = new ShadowBox(width, height, Hwnd());
		UpdateShadowBox();
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadowBox();
	}
	ShadowBox* BorderlessWindow::GetShadowBox()
	{
		return _shadowBox;
	}
	void BorderlessWindow::SetShadow(int padding)
	{
		_shadowWeight = padding;
		UpdateShadowBox();
	}
	void BorderlessWindow::DoPaint(HDC winHDC, const Rect& rePaintRect) {
		__super::DoPaint(winHDC, rePaintRect);
		if (_firstPaint) {
			_firstPaint = false;
			UpdateShadowBox();
		}
	}
	void BorderlessWindow::OnRect(const Rect& rect) {
		__super::OnRect(rect);
		if (!_firstPaint) {
			//避免先显示阴影再显示窗口的问题
			UpdateShadowBox();
		}
	}
	void BorderlessWindow::OnDpiChange(const float& systemScale, const Rect& newRect)
	{
		if (_shadowBox) {
			if (this->_shadowScale != systemScale) {
				this->_shadowWeight *= systemScale / _shadowScale;
				this->_shadowScale = systemScale;
				if (!_firstPaint) {
					UpdateShadowBox();
				}
			}
		}
		__super::OnDpiChange(systemScale, newRect);
	}
	void BorderlessWindow::Hide() {
		__super::Hide();
		UpdateShadowBox();
	}
	void BorderlessWindow::UpdateShadowBox() {
		if (_shadowBox) {
			_shadowBox->Update(_shadowWeight, this->GetLayout() ? this->GetLayout()->Style.Border.TopLeftRadius : 0);
		}
	}
	void BorderlessWindow::CloseShadowBox()
	{
		if (_shadowBox) {
			delete _shadowBox;
			_shadowBox = NULL;
		}
	}
	LRESULT  BorderlessWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
			/*case WM_NCPAINT:
			{
				return 0;
			}
			case WM_NCCALCSIZE:
			{
				return 0;
			}
			case WM_NCACTIVATE:
			{
				if (::IsIconic(Hwnd())) break;
				return (wParam == 0) ? TRUE : FALSE;
			}*/
		case WM_NCHITTEST:
		{
			//判断是否全屏
			MonitorInfo monitorInfo;
			EzUI::GetMontior(&monitorInfo, Hwnd());
			auto rect = this->GetWindowRect();
			bool isFullScreen = monitorInfo.Rect.X == rect.X && monitorInfo.Rect.Y == rect.Y && monitorInfo.Rect.Width == rect.Width && monitorInfo.Rect.Height == rect.Height;
			if (isFullScreen/* && ::GetForegroundWindow() == Hwnd()*/)
			{
				//全屏状态下不可调整窗口大小 所以break;
				break;
			}

			if (!::IsZoomed(Hwnd()) && Zoom) {
				RECT rc;
				::GetWindowRect(Hwnd(), &rc);
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
				return HTCLIENT;//
			}
			break;
		}
		case WM_KILLFOCUS: {
			HWND wnd = (HWND)wParam;
			if (this->GetShadowBox() && wnd != this->GetShadowBox()->Hwnd()) {
				this->OnKillFocus(wnd);
			}
			else if (this->GetShadowBox() == NULL) {
				this->OnKillFocus(wnd);
			}
			return 0;
		}
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
}