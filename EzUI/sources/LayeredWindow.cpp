#include "LayeredWindow.h"

namespace EzUI {

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int width, int height, HWND owner) :Window(width, height, owner, /*WS_THICKFRAME |*/ WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, WS_EX_LAYERED)
	{
		_boxShadow = new ShadowWindow(width, height, Hwnd());
		UpdateShadow();
		PublicData.InvalidateRect = [=](void* _rect) ->void {
			Rect& rect = *(Rect*)_rect;
			this->InvalidateRect(rect);
			//::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
		};
		PublicData.UpdateWindow = [=]()->void {
			if (!_InvalidateRect.IsEmptyArea()) {
				::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
			}
		};

		task = new std::thread([=]() {
			while (bRunTask)
			{
				if (!_InvalidateRect.IsEmptyArea() && this->IsVisible()) {
					::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
				}
				Sleep(1);//检测无效区域的延时
			}
			});
	}
	ShadowWindow* LayeredWindow::GetShadowWindow()
	{
		return _boxShadow;
	}
	void LayeredWindow::SetShadow(int width)
	{
		_shadowWidth = width;
		UpdateShadow();
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
			//存在隐患 导致程序无法正常退出
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
		int Width = GetClientRect().Width;
		int Height = GetClientRect().Height;
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height) {
			rect.Height = Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = Width - rect.X;
		} //这段代码是保证重绘区域一定是在窗口内
		Rect::Union(_InvalidateRect, _InvalidateRect, rect);
		//闪烁问题找到了 如果永远重绘整个客户端将不会闪烁
		//_InvalidateRect = GetClientRect();
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(sz.Width, sz.Height, EBitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	void LayeredWindow::DoPaint(HDC winHDC, const Rect& rePaintRect) {
		const Rect& clientRect = GetClientRect();//
		DXRender pt(winHDC, clientRect.X, clientRect.Y, clientRect.Width, clientRect.Height);//
		PaintEventArgs args(pt);
		args.InvalidRectangle = rePaintRect;//
		args.PublicData = &PublicData;
		args.DC = winHDC;
		OnPaint(args);//开始重绘
	}

	LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_PAINT: {
			if (_winBitmap && !_InvalidateRect.IsEmptyArea()) {
				//Debug::Info("%d %d %d %d", _InvalidateRect.X, _InvalidateRect.Y, _InvalidateRect.Width, _InvalidateRect.Height);
				_winBitmap->Earse(_InvalidateRect);//清除背景
				HDC winHDC = _winBitmap->GetDC();
				DoPaint(winHDC, _InvalidateRect);
				PushDC(winHDC);//updatelaredwindow 更新窗口
				_InvalidateRect = { 0,0,0,0 };//重置区域
			}
			return 0;
		}
		//case WM_NCPAINT:
		//{
		//	return 0;
		//}
		//case WM_NCCALCSIZE:
		//{
		//	return 0;
		//}
		//case WM_NCACTIVATE:
		//{
		//	if (::IsIconic(Hwnd())) break;
		//	return (wParam == 0) ? TRUE : FALSE;
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
		return __super::WndProc(uMsg, wParam, lParam);
	}

	void LayeredWindow::PushDC(HDC hdc) {

		const Rect& _rectClient = GetClientRect();

		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };

		BLENDFUNCTION blendFunc{ 0 };
		blendFunc.SourceConstantAlpha = 255;
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;
		blendFunc.BlendFlags = 0;
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//透明
	}
}