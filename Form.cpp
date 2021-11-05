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
			break;//窗口已经是最大化的时候不处理
		}
		RECT rc;
		GetWindowRect(_hWnd, &rc);
		POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		int x = 4;//边框宽度
		if (pt.x < rc.left + x)
		{
			if (pt.y < rc.top + x)return HTTOPLEFT;//左上角
			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//左下角
			return HTLEFT;//左边
		}
		if (pt.x >= rc.right - x)//坐标从0开始，所以使用>=
		{
			if (pt.y < rc.top + x)return HTTOPRIGHT;//右上角
			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//右下角
			return HTRIGHT;//右边
		}
		if (pt.y < rc.top + x)return HTTOP;//上边
		if (pt.y >= rc.bottom - x)return HTBOTTOM;//下边
		return HTCLIENT;//指示当前鼠标在客户区，将响应OnLButtonDown消息。
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
	_boxShadow->Update(_shadowWidth);
}
void LayeredWindow::OnRect(const Rect&rect) {

	_boxShadow->Update(_shadowWidth);
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
	StopWatch stopWatch;//计算耗时
	_layout->SetRect(this->GetClientRect(), false);
	*((Rect*)(&_layout->ClipRect)) = this->GetClientRect();//布局要先确定自己的裁剪区域是和窗口一样大小
	if (_winBitmap) {
		delete _winBitmap;
	}
	_winBitmap = new EBitmap(sz.Width, sz.Height, 32);
	OnPaint(_winBitmap->GetHDC(), _rectClient);//当窗口大小发送改变的时候重新创建DC 并且刷新
	TCHAR buf[256]{ 0 };
	sprintf_s(buf, "GDIPaint %d %d   %d ms \n", sz.Width, sz.Height, (int)stopWatch.ElapsedMilliseconds());
	OutputDebugString(buf);
}
void LayeredWindow::OnPaint(HDC _hdc, const Rect & rePaintRect) {
	Rect &clientRect = GetClientRect();//获取整体大小
	Painter pt(_hdc);//画家
	PaintEventArgs args(pt);
	args.InvalidRectangle = rePaintRect;//无效区域
	args.HWnd = _hWnd;
	_layout->OnEvent(Event::OnPaint, &args);//给布局器绘制内容

	PushDC(_hdc);//updatelaredwindow
}
LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_PAINT) //layeredWindow不允许接受WM_PAINT消息
	{
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_CONTROL_REFRESH && _winBitmap) {
		Control *ctl = (Control*)wParam;
		OnPaint(_winBitmap->GetHDC(), ctl->GetClientRect());//控件局部刷新
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_NCHITTEST) {
		if (::IsZoomed(_hWnd)) {
			return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);//窗口已经是最大化的时候不处理
		}
		RECT rc;
		GetWindowRect(_hWnd, &rc);
		POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		int x = 4;//边框宽度
		if (pt.x < rc.left + x)
		{
			if (pt.y < rc.top + x)return HTTOPLEFT;//左上角
			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//左下角
			return HTLEFT;//左边
		}
		if (pt.x >= rc.right - x)//坐标从0开始，所以使用>=
		{
			if (pt.y < rc.top + x)return HTTOPRIGHT;//右上角
			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//右下角
			return HTRIGHT;//右边
		}
		if (pt.y < rc.top + x)return HTTOP;//上边
		if (pt.y >= rc.bottom - x)return HTBOTTOM;//下边
		return HTCLIENT;//指示当前鼠标在客户区，将响应OnLButtonDown消息。
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
	UpdateLayeredWindow(_hWnd, NULL, NULL, &size, hdc, &point, 0, &blend, LWA_ALPHA);//。。。。。。更新分层窗口
}

MenuWindow::MenuWindow(int cx, int cy, HWND owner) :BorderlessWindow(cx, cy, owner)
{

}

LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS && !frist) {
		Debug::Log("失去焦点 关闭窗口");
		this->Close();
	}
	else if (uMsg == WM_KILLFOCUS) {
		frist = false;
	}
	return __super::WndProc(uMsg, wParam, lParam);
}
