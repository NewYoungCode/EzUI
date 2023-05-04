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
				::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
			}
		};
		task = new std::thread([=]() {
			while (bRunTask)
			{
				if (!_InvalidateRect.IsEmptyArea()) {
					::PostMessage(Hwnd(), WM_PAINT, NULL, NULL);
				}
				Sleep(5);//�����Ч�������ʱ 200fps
			}
			});
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
		} //��δ����Ǳ�֤�ػ�����һ�����ڴ�����
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
	void LayeredWindow::OnPaint(PaintEventArgs& args) {
		if (MainLayout) {
			MainLayout->Rending(args);//
		}
	}

	void LayeredWindow::Rending(HDC winHDC, const Rect& rePaintRect) {
		Rect& clientRect = GetClientRect();//
		Painter pt(winHDC, clientRect.X, clientRect.Y, clientRect.Width, clientRect.Height);//
		PaintEventArgs args(pt);
		args.InvalidRectangle = _InvalidateRect;//
		args.PublicData = &PublicData;
		args.DC = winHDC;
		OnPaint(args);//��ʼ�ػ�
	}

	LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_PAINT) //layeredWindow
		{
			if (_winBitmap) {
				_winBitmap->Earse(_InvalidateRect);//�������
				HDC winHDC = _winBitmap->GetDC();
				Rending(winHDC, _InvalidateRect);
				PushDC(winHDC);//updatelaredwindow ���´���
				_InvalidateRect = { 0,0,0,0 };//��������
				return 0;
			}
			return ::DefWindowProc(Hwnd(), uMsg, wParam, lParam);
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
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_OPAQUE);//��͸��
		//::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//͸��
	}
}