#include "LayeredWindow.h"

namespace EzUI {
	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int_t width, int_t height, HWND owner) :BorderlessWindow(width, height, owner, WS_EX_LAYERED)
	{
		UpdateShadowBox();
		this->PublicData->InvalidateRect = [this](const Rect& rect) ->void {
			this->InvalidateRect(rect);
			bool exist = false;
			MSG msg;
			// 先把所有 WM_PAINT 的消息都移除（避免堆积多条）
			while (PeekMessage(&msg, Hwnd(), WM_PAINT, WM_PAINT, PM_REMOVE)) {
				// 这里可以处理消息，或者什么都不做，直接移除
				int pause = 0;
				exist = true;
			}
			::PostMessage(Hwnd(), WM_PAINT, 0, 0);
			};
		this->PublicData->UpdateWindow = [this]()->void {
			//实时绘制
			if (IsVisible() && !_invalidateRect.IsEmptyArea()) {
				this->Paint();
			}
			};
	}

	LayeredWindow::~LayeredWindow() {
		if (_winBitmap) {
			delete _winBitmap;
		}
	}
	Rect LayeredWindow::GetUpdateRect()
	{
		return _invalidateRect;
	}
	void LayeredWindow::InvalidateRect(const Rect& _rect) {
		const Rect& clientRect = GetClientRect();
		int_t Width = clientRect.Width;
		int_t Height = clientRect.Height;
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
		Rect::Union(_invalidateRect, _invalidateRect, rect);
	}
	LRESULT LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_PAINT) {
			//处理分层窗口的绘制
			this->Paint();
			return TRUE;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new Bitmap(sz.Width, sz.Height, Bitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	void LayeredWindow::Paint()
	{
		//_invalidateRect = Rect(0, 0, this->GetClientRect().Width, this->GetClientRect().Height);
		if (_winBitmap && !_invalidateRect.IsEmptyArea()) {

			_winBitmap->Earse(_invalidateRect);//清除背景
			HDC winHDC = _winBitmap->GetHDC();

#if 0
			//不使用双缓冲
			DoPaint(winHDC, _invalidateRect);
#else
			//使用双缓冲
			Bitmap doubleBuff(_winBitmap->Width(), _winBitmap->Height(), Bitmap::PixelFormat::PixelFormatARGB);
			DoPaint(doubleBuff.GetHDC(), _invalidateRect);
			//使用BitBlt函数进行复制到winHDC  //如果窗体不规则 不适用于BitBlt进行复制
			::BitBlt(winHDC, _invalidateRect.X, _invalidateRect.Y,
				_invalidateRect.Width, _invalidateRect.Height,
				doubleBuff.GetHDC(), _invalidateRect.X, _invalidateRect.Y,
				SRCCOPY);
#endif
			PushDC(winHDC);//updatelaredwindow 更新窗口
			_invalidateRect = Rect();//重置区域
		}
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