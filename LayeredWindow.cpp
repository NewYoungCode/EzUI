#include "LayeredWindow.h"

namespace EzUI {
	LRESULT __ZoomWindow(const HWND& _hWnd, const  LPARAM& lParam) {
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
	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int cx, int cy, HWND owner) :Window(cx, cy, owner, WS_POPUP | WS_MINIMIZEBOX, WS_EX_LAYERED)
	{
		_boxShadow = new ShadowWindow(cx, cy, _hWnd);
		UpdateShadow();
		_winData.InvalidateRect = [=](void* _rect) ->void {
			this->InvalidateRect(*(Rect*)_rect);
		};
		_winData.UpdateWindow = [=]()->void {
			if (!_InvalidateRect.IsEmptyArea()) {
				::SendMessage(_hWnd, UI_PAINT, NULL, NULL);
			}
		};
		task = new std::thread([=]() {
			while (bRunTask)
			{
				if (!_InvalidateRect.IsEmptyArea()) {
					::SendMessage(_hWnd, UI_PAINT, NULL, NULL);
				}
				Sleep(5);//检测无效区域的延时 200fps
			}
			});
	}
	void LayeredWindow::OnRect(const Rect& rect) {
		UpdateShadow();
		__super::OnRect(rect);
	}
	void LayeredWindow::Hide() {
		__super::Hide();
		UpdateShadow();
	}
	LayeredWindow::~LayeredWindow() {
		if (bRunTask) {
			bRunTask = false;
			task->join();
			delete task;
			task = NULL;
		}
		CloseShadow();
	}
	void LayeredWindow::UpdateShadow() {
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
	}
	void LayeredWindow::CloseShadow()
	{
		if (_boxShadow) {
			delete _boxShadow;
			_boxShadow = NULL;
		}
	}
	void LayeredWindow::InvalidateRect(const Rect& _rect) {
		if (_InvalidateRect.IsEmptyArea()) {
			_InvalidateRect = _rect;
		}
		else
		{
			Rect::Union(_InvalidateRect, _InvalidateRect, _rect);
		}
	}
	void LayeredWindow::OnSize(const Size& sz) {
		auto rect = this->GetClientRect();
		*((Rect*)(&MainLayout->ClipRect)) = rect;//
		MainLayout->SetRect(rect, false);
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(rect.Width, rect.Height);
		this->InvalidateRect(rect);
	}
	void LayeredWindow::OnPaint(HDC _hdc, const Rect& rePaintRect) {
		Rect& clientRect = GetClientRect();//
		Painter pt(_hdc);//
		PaintEventArgs args(pt);
		args.InvalidRectangle = rePaintRect;//
		args.HWnd = _hWnd;
		MainLayout->OnEvent(Event::OnPaint, &args);//
		PushDC(_hdc);//updatelaredwindow
	}
	LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_PAINT)
		{
			PAINTSTRUCT pst;//也许这样能避免锁屏之后一直发送WM_PAINT消息的问题
			BeginPaint(_hWnd, &pst);
			RECT& r = pst.rcPaint;
			EndPaint(_hWnd, &pst);
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
		}
		if (uMsg == UI_PAINT) //layeredWindow
		{
			if (_winBitmap) {
				OnPaint(_winBitmap->GetDC(), _InvalidateRect);
				_InvalidateRect = { 0,0,0,0 };
			}
			return FALSE;
		}
		if (uMsg == WM_NCHITTEST) {
			if (!::IsZoomed(_hWnd) && Zoom) {
				return __ZoomWindow(_hWnd, lParam);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}

	void LayeredWindow::PushDC(HDC hdc) {
		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };
		BLENDFUNCTION blend;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 255;
		UpdateLayeredWindow(_hWnd, NULL, NULL, &size, hdc, &point, 0, &blend, ULW_OPAQUE);//
	}
}