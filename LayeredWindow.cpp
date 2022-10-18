#include "LayeredWindow.h"

namespace EzUI {

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int cx, int cy, HWND owner) :Window(cx, cy, owner, WS_POPUP | WS_MINIMIZEBOX, WS_EX_LAYERED)
	{
		_boxShadow = new ShadowWindow(cx, cy, Hwnd());
		UpdateShadow();
		PublicData.InvalidateRect = [=](void* _rect) ->void {
			this->InvalidateRect(*(Rect*)_rect);
		};
		PublicData.UpdateWindow = [=]()->void {
			if (!_InvalidateRect.IsEmptyArea()) {
				::SendMessage(Hwnd(), UI_PAINT, NULL, NULL);
			}
		};
		task = new std::thread([=]() {
			while (bRunTask)
			{
				if (!_InvalidateRect.IsEmptyArea()) {
					::SendMessage(Hwnd(), UI_PAINT, NULL, NULL);
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
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(sz.Width, sz.Height, EBitmap::PixelFormat::PixelFormatARGB);
		MainLayout->SetRect(this->GetClientRect());
		MainLayout->Invalidate();
	}
	void LayeredWindow::OnPaint(HDC _hdc, const Rect& rePaintRect) {
		Rect& clientRect = GetClientRect();//
		Painter pt(_hdc, clientRect.Width, clientRect.Height);//
		PaintEventArgs args(pt);
		args.InvalidRectangle = rePaintRect;//
		args.PublicData = &PublicData;
		args.DC = _hdc;
		MainLayout->Rending(args);//
	}
	LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_PAINT)
		{
			PAINTSTRUCT pst;//也许这样能避免锁屏之后一直发送WM_PAINT消息的问题
			BeginPaint(Hwnd(), &pst);
			RECT& r = pst.rcPaint;
			EndPaint(Hwnd(), &pst);
			return ::DefWindowProc(Hwnd(), uMsg, wParam, lParam);
		}
		if (uMsg == UI_PAINT) //layeredWindow
		{
			if (_winBitmap) {
				_winBitmap->Earse(_InvalidateRect);//清除背景
				OnPaint(_winBitmap->GetDC(), _InvalidateRect);//开始重绘
				PushDC(_winBitmap->GetDC());//updatelaredwindow 更新窗口
				_InvalidateRect = { 0,0,0,0 };//重置区域
			}
			return FALSE;
		}
		if (uMsg == WM_NCHITTEST) {
			if (!::IsZoomed(Hwnd()) && Zoom) {
				return ZoomWindow(lParam);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}

	void LayeredWindow::PushDC(HDC hdc) {

		Rect& _rectClient = GetClientRect();

		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };

		BLENDFUNCTION blendFunc{ 0 };
		blendFunc.SourceConstantAlpha = 255;
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;
		blendFunc.BlendFlags = 0;
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_OPAQUE);//不透明
		//::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//透明
	}
}