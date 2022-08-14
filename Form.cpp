#include "Form.h"
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
	_boxShadow->Update(_shadowWidth);
}
void BorderlessWindow::OnRect(const Rect& rect) {
	_boxShadow->Update(_shadowWidth);
	__super::OnRect(rect);
}
void BorderlessWindow::Hide() {
	__super::Hide();
	if (_boxShadow) {
		_boxShadow->Update(_shadowWidth);
	}
}
BorderlessWindow::~BorderlessWindow() {
	if (_boxShadow) {
		delete _boxShadow;
	}
}

LRESULT  BorderlessWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg)
	{
	case WM_NCHITTEST: {
		if (!Zoom) {
			break;
		}
		if (::IsZoomed(_hWnd)) {
			break;//
		}
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
	if (_boxShadow) {
		delete _boxShadow;
	}
}

void LayeredWindow::OnSize(const Size& sz) {
	StopWatch stopWatch;//
	_layout->SetRect(this->GetClientRect(), false);
	*((Rect*)(&_layout->ClipRect)) = this->GetClientRect();//
	if (_winBitmap) {
		delete _winBitmap;
	}
	_winBitmap = new EBitmap(sz.Width, sz.Height, 32);
	OnPaint(_winBitmap->GetHDC(), _rectClient);//
	CHAR buf[256]{ 0 };
	sprintf_s(buf, "GDIPaint %d %d   %d ms \n", sz.Width, sz.Height, (int)stopWatch.ElapsedMilliseconds());
	OutputDebugStringA(buf);
}
void LayeredWindow::OnPaint(HDC _hdc, const Rect& rePaintRect) {
	Rect& clientRect = GetClientRect();//
	Painter pt(_hdc);//
	PaintEventArgs args(pt);
	args.InvalidRectangle = rePaintRect;//
	args.HWnd = _hWnd;
	_layout->OnEvent(Event::OnPaint, &args);//

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
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_NCHITTEST) {
		if (::IsZoomed(_hWnd)) {
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);//
		}
		RECT rc;
		GetWindowRect(_hWnd, &rc);
		POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		int x = 4;//
		if (pt.x < rc.left + x)
		{
			if (pt.y < rc.top + x)return HTTOPLEFT;//x
			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//
			return HTLEFT;//
		}
		if (pt.x >= rc.right - x)
		{
			if (pt.y < rc.top + x)return HTTOPRIGHT;//
			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;
			return HTRIGHT;//
		}
		if (pt.y < rc.top + x)return HTTOP;//
		if (pt.y >= rc.bottom - x)return HTBOTTOM;//
		return HTCLIENT;//ָ
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
