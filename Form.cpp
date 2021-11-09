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
void BorderlessWindow::OnRect(const Rect&rect) {
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
void LayeredWindow::OnRect(const Rect&rect) {
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

void LayeredWindow::OnSize(const Size&sz) {
	StopWatch stopWatch;//�����ʱ
	_layout->SetRect(this->GetClientRect(), false);
	*((Rect*)(&_layout->ClipRect)) = this->GetClientRect();//����Ҫ��ȷ���Լ��Ĳü������Ǻʹ���һ����С
	if (_winBitmap) {
		delete _winBitmap;
	}
	_winBitmap = new EBitmap(sz.Width, sz.Height, 32);
	OnPaint(_winBitmap->GetHDC(), _rectClient);//�����ڴ�С���͸ı��ʱ�����´���DC ����ˢ��
	CHAR buf[256]{ 0 };
	sprintf_s(buf, "GDIPaint %d %d   %d ms \n", sz.Width, sz.Height, (int)stopWatch.ElapsedMilliseconds());
	OutputDebugStringA(buf);
}
void LayeredWindow::OnPaint(HDC _hdc, const Rect & rePaintRect) {
	Rect &clientRect = GetClientRect();//��ȡ�����С
	Painter pt(_hdc);//����
	PaintEventArgs args(pt);
	args.InvalidRectangle = rePaintRect;//��Ч����
	args.HWnd = _hWnd;
	_layout->OnEvent(Event::OnPaint, &args);//����������������

	PushDC(_hdc);//updatelaredwindow
}
LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_PAINT) //layeredWindow���������WM_PAINT��Ϣ
	{
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_CONTROL_REFRESH && _winBitmap) {
		Control *ctl = (Control*)wParam;
		OnPaint(_winBitmap->GetHDC(), ctl->GetClientRect());//�ؼ��ֲ�ˢ��
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_NCHITTEST) {
		if (::IsZoomed(_hWnd)) {
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);//�����Ѿ�����󻯵�ʱ�򲻴���
		}
		RECT rc;
		GetWindowRect(_hWnd, &rc);
		POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		int x = 4;//�߿���
		if (pt.x < rc.left + x)
		{
			if (pt.y < rc.top + x)return HTTOPLEFT;//���Ͻ�
			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//���½�
			return HTLEFT;//���
		}
		if (pt.x >= rc.right - x)//�����0��ʼ������ʹ��>=
		{
			if (pt.y < rc.top + x)return HTTOPRIGHT;//���Ͻ�
			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//���½�
			return HTRIGHT;//�ұ�
		}
		if (pt.y < rc.top + x)return HTTOP;//�ϱ�
		if (pt.y >= rc.bottom - x)return HTBOTTOM;//�±�
		return HTCLIENT;//ָʾ��ǰ����ڿͻ���������ӦOnLButtonDown��Ϣ��
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
	UpdateLayeredWindow(_hWnd, NULL, NULL, &size, hdc, &point, 0, &blend, LWA_ALPHA);//���������������·ֲ㴰��
}

MenuWindow::MenuWindow(int cx, int cy, HWND owner) :BorderlessWindow(cx, cy, owner)
{

}

LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS && !frist) {
		//Debug::Log(TEXT("ʧȥ���� �رմ���"));
		this->Close();
	}
	else if (uMsg == WM_KILLFOCUS) {
		frist = false;
	}
	return __super::WndProc(uMsg, wParam, lParam);
}
