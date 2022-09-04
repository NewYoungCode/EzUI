#include "Form.h"

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
	void setA(const Rect& clipRect, EBitmap* _bufBitmap, int x, int y, BYTE a) {
		if (clipRect.Contains(x, y)) { //不允许绘制在OWner窗口区域
			return;
		}
		DWORD* point = (DWORD*)_bufBitmap->point + (x + y * _bufBitmap->Width);//起始地址+坐标偏移
		((BYTE*)point)[3] = a;//修改A通道数值
	}
	bool SetShadow(const Rect& clipRect, EBitmap* _bufBitmap, int m_Width, int m_Height, size_t iSize) {
		int width = m_Width < m_Height ? m_Width : m_Height;
		int radius = 3;//半径
		int max_size = width / 2 - radius;
		if (max_size <= 0) {
			radius = 0;
			max_size = width / 2;
		}
		iSize = (int)iSize < max_size ? iSize : max_size;
		double piAngle = 3.1415926;
		int iSizeB = 4 * iSize;
		double fN = piAngle / iSize / 4.1;//设置四条边外模糊度
		double lN = 1.0 / iSize;
		int iAplpha = 0;
		int Left = iSize + radius,
			Top = iSize + radius,
			Right = m_Width - iSize - radius,
			Bottom = m_Height - iSize - radius;
		int x = 0, y = 0;
		for (size_t i = 0; i < iSize; i++)
		{
			iAplpha = int(255 - cos(fN * (i)) * 255);
			for (x = Left; x <= Right; x++)
			{
				setA(clipRect, _bufBitmap, x, i, iAplpha);
				setA(clipRect, _bufBitmap, x, m_Height - i - 1, iAplpha);
			}
			for (y = Top; y <= Bottom; y++)
			{
				setA(clipRect, _bufBitmap, i, y, iAplpha);
				setA(clipRect, _bufBitmap, m_Width - i - 1, y, iAplpha);
			}
		}
		double fL = 0;
		int iSizeR = iSize + radius;
		for (int y = 0; y < iSizeR; y++)
		{
			for (int x = y; x < iSizeR; x++)
			{
				fL = sqrt((iSizeR - x) * (iSizeR - x) + (iSizeR - y) * (iSizeR - y));
				if (fL <= radius) {
					break;
				}
				if (fL > radius && fL < iSizeR) {
					iAplpha = int(255 - cos(fN * (iSizeR - fL)) * 255);
				}
				else {
					iAplpha = 0;
				}
				setA(clipRect, _bufBitmap, x, y, iAplpha);
				setA(clipRect, _bufBitmap, y, x, iAplpha);
				setA(clipRect, _bufBitmap, m_Width - x - 1, y, iAplpha);
				setA(clipRect, _bufBitmap, m_Width - y - 1, x, iAplpha);
				setA(clipRect, _bufBitmap, x, m_Height - y - 1, iAplpha);
				setA(clipRect, _bufBitmap, y, m_Height - x - 1, iAplpha);
				setA(clipRect, _bufBitmap, m_Width - x - 1, m_Height - y - 1, iAplpha);
				setA(clipRect, _bufBitmap, m_Width - y - 1, m_Height - x - 1, iAplpha);
			}
		}
		return true;
	}

	FrameWindow::FrameWindow(int cx, int cy, HWND owner) :Window(cx, cy, owner, WS_OVERLAPPEDWINDOW, NULL)
	{
	}
	BorderlessWindow::BorderlessWindow(int width, int height, HWND owner) : Window(width, height, owner, WS_POPUP | WS_MINIMIZEBOX, NULL)
	{
		_boxShadow = new BoxShadow(width, height, _hWnd);
		_boxShadow->Update(_shadowWidth);
	}

	void BorderlessWindow::SetShadow(int width)
	{
		_shadowWidth = width;
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
	}
	void BorderlessWindow::OnRect(const Rect& rect) {
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
		__super::OnRect(rect);
	}
	void BorderlessWindow::Hide() {
		__super::Hide();
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadow();
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
		case WM_NCCALCSIZE:
		{
			LPNCCALCSIZE_PARAMS ncParams = (LPNCCALCSIZE_PARAMS)lParam;
			int borderWidth = 20;
			ncParams->rgrc[0].top += borderWidth;
			ncParams->rgrc[0].left += borderWidth;
			ncParams->rgrc[0].bottom -= borderWidth;
			ncParams->rgrc[0].right -= borderWidth;
			return 0;
			break;
		}
		case WM_NCPAINT:
		{
#ifndef DCX_USESTYLE
#define DCX_USESTYLE 0x00010000
#endif
			HDC hdc = ::GetDCEx(_hWnd, 0, DCX_WINDOW | DCX_USESTYLE);
			if (hdc) {
				RECT rcclient;
				::GetClientRect(_hWnd, &rcclient);
				RECT rcwin;
				::GetWindowRect(_hWnd, &rcwin);
				POINT ptupleft;
				ptupleft.x = rcwin.left;
				ptupleft.y = rcwin.top;
				::MapWindowPoints(0, _hWnd, (LPPOINT)&rcwin, (sizeof(RECT) / sizeof(POINT)));
				::OffsetRect(&rcclient, -rcwin.left, -rcwin.top);
				::OffsetRect(&rcwin, -rcwin.left, -rcwin.top);

				HRGN rgntemp = NULL;
				if (wParam == NULLREGION || wParam == ERROR) {
					::ExcludeClipRect(hdc, rcclient.left, rcclient.top, rcclient.right, rcclient.bottom);
				}
				else {
					rgntemp = ::CreateRectRgn(rcclient.left + ptupleft.x, rcclient.top + ptupleft.y, rcclient.right + ptupleft.x, rcclient.bottom + ptupleft.y);
					if (::CombineRgn(rgntemp, (HRGN)wParam, rgntemp, RGN_DIFF) == NULLREGION) {
						// nothing to paint
					}
					::OffsetRgn(rgntemp, -ptupleft.x, -ptupleft.y);
					::ExtSelectClipRgn(hdc, rgntemp, RGN_AND);
				}

				Rect rect2(rcwin);
				Rect clipRect(rcclient);

				EBitmap eBitmap(rect2.Width, rect2.Height, 32);
				EzUI::SetShadow(clipRect, &eBitmap, rect2.Width, rect2.Height, 20);

				Painter pt(eBitmap.GetHDC());
				pt.FillRectangle(Color::Pink, rect2,20);

				SaveHDCToFile(eBitmap.GetHDC(), &rcwin, L"image/png", L"d:/aa.png");

				BLENDFUNCTION blend;
				blend.BlendOp = AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.AlphaFormat = AC_SRC_ALPHA;
				blend.SourceConstantAlpha = 255;
				::StretchBlt(hdc, 0, 0, 1000,670, eBitmap.GetHDC(), 0, 0, clipRect.Width, clipRect.Height, SRCCOPY);
				//::BitBlt(hdc, 0, 0, rect2.Width, rect2.Height, eBitmap.GetHDC(), 0, 0, SRCCOPY);
				HBRUSH hbrush = ::CreateSolidBrush(RGB(255, 255, 0));
				//::FillRect(hdc, &rcwin, hbrush);
				::ReleaseDC(_hWnd, hdc);
			}
			return 0;
			//RECT rect;
			//GetWindowRect(_hWnd, &rect);
			//RECT rect2;
			//::GetClientRect(_hWnd, &rect2);
			//HDC hdc;
			//hdc = GetDCEx(_hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN);
			//// Paint into this DC 
			//Painter pt(hdc);
			//pt.FillRectangle(Color::Red, rect2);
			//ReleaseDC(_hWnd, hdc);
			//return 0;
		}
		case WM_NCACTIVATE:
		{
			RedrawWindow(_hWnd, NULL, NULL, RDW_UPDATENOW);
			return 0;
			break;
		}

		case WM_NCHITTEST: {
			if (!::IsZoomed(_hWnd) && Zoom) {
				return __ZoomWindow(_hWnd, lParam);
			}
			break;
		}
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int cx, int cy, HWND owner) :Window(cx, cy, owner, WS_POPUP | WS_MINIMIZEBOX, WS_EX_LAYERED)
	{
		_boxShadow = new BoxShadow(cx, cy, _hWnd);
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
	void LayeredWindow::OnRect(const Rect& rect) {
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
		__super::OnRect(rect);
	}
	void LayeredWindow::Hide() {
		__super::Hide();
		if (_boxShadow) {
			_boxShadow->Update(_shadowWidth);
		}
	}
	LayeredWindow::~LayeredWindow() {
		CloseShadow();
	}

	void LayeredWindow::OnSize(const Size& sz) {
		auto rect = this->GetClientRect();
		*((Rect*)(&MainLayout->ClipRect)) = rect;//
		MainLayout->SetRect(rect, false);
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(rect.Width, rect.Height, 32);
		OnPaint(_winBitmap->GetHDC(), _rectClient);//
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
		if (uMsg == WM_PAINT) //layeredWindow
		{
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
		}
		if (uMsg == UI_CONTROL_REFRESH && _winBitmap) {
			Control* ctl = (Control*)wParam;
			OnPaint(_winBitmap->GetHDC(), ctl->GetClientRect());//
			return TRUE;
		}
		if (uMsg == WM_NCHITTEST) {
			if (!::IsZoomed(_hWnd) && Zoom) {
				return __ZoomWindow(_hWnd, lParam);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	//UpdateLayeredWindow
	void LayeredWindow::PushDC(HDC hdc) {
		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };
		BLENDFUNCTION blend;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 255;
		UpdateLayeredWindow(_hWnd, NULL, NULL, &size, hdc, &point, 0, &blend, LWA_ALPHA);//
	}

	MenuWindow::MenuWindow(int cx, int cy, HWND owner) :BorderlessWindow(cx, cy, owner)
	{
	}
	LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_NCPAINT)
		{
			HWND hWnd = _hWnd;
			HDC hdc;
			hdc = GetWindowDC(hWnd);
			// Paint into this DC
			RECT rcWin;
			GetWindowRect(hWnd, &rcWin);
			OffsetRect(&rcWin, -rcWin.left, -rcWin.top);
			auto b = CreateSolidBrush(RGB(255, 0, 0));

			for (int i = 0; i < 4; i++)
			{
				FrameRect(hdc, &rcWin, b);
				InflateRect(&rcWin, -1, -1);
			}
			ReleaseDC(hWnd, hdc);
			return 0;
		}

		if (uMsg == WM_NCHITTEST) {
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
		}
		if (uMsg == WM_KILLFOCUS) {
			HWND wnd = (HWND)wParam;
			if (wnd != this->_boxShadow->_hWnd) {
				this->Close();
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void MenuWindow::Show(int cmdShow)
	{
		HWND Owner = ::GetWindowOwner(_hWnd);
		RECT OwnerRect;
		::GetWindowRect(Owner, &OwnerRect);
		auto rect = GetRect();

		POINT mousePos;
		GetCursorPos(&mousePos);
		POINT clientPos = mousePos;
		ScreenToClient(Owner, &clientPos);
		Debug::Log("%d %d", clientPos.x, clientPos.y);

		int height = OwnerRect.bottom - OwnerRect.top;
		//if (rect.Height>height  ) {
		::MoveWindow(_hWnd, mousePos.x, mousePos.y, rect.Width, rect.Height, FALSE);
		/*}
		else {
			::MoveWindow(_hWnd, mousePos.x, mousePos.y- rect.Height, rect.Width, rect.Height, FALSE);
		}*/
		__super::Show(cmdShow);
		::SetForegroundWindow(_hWnd);
	}

};