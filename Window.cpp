#include "Window.h"

Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
{
	int sw = GetSystemMetrics(SM_CXFULLSCREEN);
	int sh = GetSystemMetrics(SM_CYFULLSCREEN);
	_rect.X = (sw - width) / 2;
	_rect.Y = (sh - height) / 2;
	_rect.Width = width;
	_rect.Height = height;
	_hWnd = ::CreateWindowEx(ExStyle, UI_CLASSNAME, UI_CLASSNAME, dStyle,
		_rect.X, _rect.Y, width, height, owner, NULL, GetModuleHandle(NULL), NULL);
	UI_SetUserData(_hWnd, this);
}
Window::~Window()
{
	if (::IsWindow(_hWnd)) {
		::DestroyWindow(_hWnd);
	}
}

Control* Window::FindControl(const EString& objectName)
{
	return this->_layout->FindControl(objectName);
}


HWND Window::Hwnd()
{
	return _hWnd;
}
Rect& Window::GetRect()
{
	if (_rect.IsEmptyArea()) {
		RECT rect;
		::GetWindowRect(_hWnd, &rect);
		_rect = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
	}
	return _rect;
}
Rect& Window::GetClientRect()
{
	if (_rectClient.IsEmptyArea()) {
		RECT rect;
		::GetClientRect(_hWnd, &rect);
		_rectClient = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
	}
	return _rectClient;
}
void Window::SetText(const EString& text)
{
	::SetWindowTextW(_hWnd, text.utf16().c_str());
}
void Window::ReSize(const Size& size) {
	RECT rect;
	::GetWindowRect(_hWnd, &rect);
	::MoveWindow(_hWnd, rect.left, rect.right, size.Width, size.Height, TRUE);
}
void Window::SetIcon(short id)
{
	SetIcon(::LoadIcon(::GetModuleHandle(0), MAKEINTRESOURCE(id)));//
}
void Window::SetIcon(HICON icon)
{
	::SendMessageW(_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
}
void Window::SetLayout(Layout* layout) {
	ASSERT(layout);
	_layout = layout;
	if (_layout->Style.FontFamily.empty()) {
		_layout->Style.FontFamily = TEXT("Microsoft YaHei");
	}
	if (!_layout->Style.FontSize.valid) {
		_layout->Style.FontSize = 9;
	}
	if (!_layout->Style.ForeColor.valid) {
		_layout->Style.ForeColor = Color::Black;
	}
	_layout->_hWnd = _hWnd;
	_layout->SetRect(this->GetClientRect());
}
void Window::Close(int code) {
	_closeCode = code;
	::SendMessageW(_hWnd, WM_CLOSE, 0, 0);
}
void Window::Show(int cmdShow)
{
	ASSERT(_layout);
	::ShowWindow(_hWnd, cmdShow);
	//MONITORINFO oMonitor = {};
	//oMonitor.cbSize = sizeof(oMonitor);
	//::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	//rcArea = oMonitor.rcWork;
}

int Window::ShowModal(bool wait)
{
	auto p_hwnd = ::GetWindowOwner(_hWnd);
	ASSERT(::IsWindow(p_hwnd));
	::EnableWindow(p_hwnd, FALSE);
	this->Show();
	::SetForegroundWindow(_hWnd);
	if (wait) {//
		MSG msg;
		while (::IsWindow(_hWnd) && ::GetMessage(&msg, 0, 0, 0) && msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {//
			::PostQuitMessage(msg.wParam);
		}
	}
	::EnableWindow(p_hwnd, TRUE);
	::SetForegroundWindow(p_hwnd);
	return _closeCode;
}

void Window::Hide() {
	::ShowWindow(_hWnd, SW_HIDE);
}
bool Window::IsVisible() {
	return ::IsWindowVisible(_hWnd) ? true : false;
}
void Window::SetVisible(bool flag) {
	if (flag) {
		::ShowWindow(_hWnd, SW_RESTORE);
		::SetForegroundWindow(_hWnd);
	}
	else {
		Hide();
	}
}

void Window::EmptyControl(Controls* controls) {
	//_focusControl = NULL;
	//_inputControl = NULL;
	//return;
	for (auto it : *controls) {
		if (_focusControl == it) {
			_focusControl = NULL;
			Debug::Log("EmptyControl %p", it);
		}
		if (_inputControl == it) {
			_inputControl = NULL;
		}
		EmptyControl((Controls*)it->GetControls());
	}
}
//#pragma comment(lib,"odbc32.lib")
//#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"imm32.lib")
LRESULT HandleStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

LRESULT  Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == uMsg || uMsg == WM_CTLCOLOREDIT) {
		Control* win32Control = (Control*)UI_GetUserData((HWND)lParam);
		EditWnd* win32Edit = dynamic_cast<EditWnd*>(win32Control);
		if (win32Edit) {
			return win32Edit->WndProc(uMsg, wParam, lParam);
		}
	}

	switch (uMsg)
	{
	case  WM_IME_STARTCOMPOSITION://
	{
		HIMC hIMC = ImmGetContext(_hWnd);
		COMPOSITIONFORM cpf;
		cpf.dwStyle = CFS_POINT;
		int x = 0;
		int y = 0;
		if (_inputControl) {
			auto& ime = *_inputControl;
			x = ime.GetClientRect().X;
			y = ime.GetClientRect().Y + ime.GetClientRect().Height;
		}
		else
		{
			return TRUE;
		}
		cpf.ptCurrentPos.x = x;
		cpf.ptCurrentPos.y = y;
		ImmSetCompositionWindow(hIMC, &cpf);
		ImmReleaseContext(_hWnd, hIMC);
		break;
	}
	case WM_IME_COMPOSITION:
	{
		HIMC        hIMC;
		ImmGetContext(_hWnd);
		hIMC = ImmGetContext(_hWnd);
		char buf[256]{ 0 };
		BOOL b = ImmGetCompositionString(hIMC, GCS_COMPSTR, buf, 255);
		b = ImmReleaseContext(_hWnd, hIMC);
		break;
	}
	case WM_ERASEBKGND: {
		return TRUE;
	}
	case UI_CONTROL_REFRESH: {
		Control* ctl = (Control*)wParam;
		Rect clienRect = ctl->GetClientRect();
		RECT wRect;
		wRect.left = clienRect.X;
		wRect.top = clienRect.Y;
		wRect.right = clienRect.GetRight();
		wRect.bottom = clienRect.GetBottom();
		::InvalidateRect(_hWnd, &wRect, FALSE);
		return TRUE;
	}
	case WM_PAINT:
	{
		if (!_load) {
			_load = true;
			OnLoad();
		}
		PAINTSTRUCT pst;
		BeginPaint(_hWnd, &pst);
		RECT& r = pst.rcPaint;
		Rect paintRect{ r.left,r.top,r.right - r.left, r.bottom - r.top };
		OnPaint(pst.hdc, paintRect);
		EndPaint(_hWnd, &pst);
		break;
	}
	case  UI_CONTROL_DELETE:
	{
		Control* delControl = (Control*)wParam;
		if (_focusControl == delControl) {
			_focusControl = NULL;
			return TRUE;
		}
		if (_inputControl == delControl) {
			_focusControl = NULL;
			return TRUE;
		}
		EmptyControl((Controls*)delControl->GetControls());
		return TRUE;
	}
	case WM_WINDOWPOSCHANGED: {
		bool rePaint = false;
		WINDOWPOS* wPos = (WINDOWPOS*)(void*)lParam;
		int flag = SWP_NOCOPYBITS;//
		if ((wPos->flags & flag) == flag) {
			rePaint = true;
		}
		RECT rect;
		::GetClientRect(_hWnd, &rect);
		Point point{ rect.left,rect.top };
		Size size{ rect.right - rect.left,rect.bottom - rect.top };
		_rectClient = { point,size };
		if (_rectClient.IsEmptyArea()) {
			_rect = { 0,0,0,0 };
			return TRUE;
		}
		_rect = { wPos->x,wPos->y,wPos->cx,wPos->cy };
		if (!_lastSize.Equals(size)) {
			_lastSize = size;
			OnSize(size);
		}
		else if (rePaint) {
			Debug::Log("SWP_NOCOPYBITS!");
			OnSize(size);
		}
		if (!_lastPoint.Equals(point)) {
			_lastPoint = point;
			OnMove(point);
		}
		OnRect(_rect);
		return TRUE;
	}
	case WM_CLOSE:
	{
		bool Cancel = false;
		OnClose(Cancel);
		if (!Cancel) {
			::DestroyWindow(_hWnd);
		}
		else {
			return TRUE;
		}
		break;
	}
	case SWP_HIDEWINDOW: {
		break;
	}
	case WM_CHAR: {
		OnChar(wParam, lParam);
		break;
	}
	case WM_KEYDOWN:
	{
		OnKeyDown(wParam);
		break;
	}
	case WM_DESTROY:
	{
		OnDestroy();
		break;
	}
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = _hWnd;
		TrackMouseEvent(&tme);
		OnMouseMove({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		if (_mouseIn == false) {
			_mouseIn = true;
		}
		break;
	}
	//case WM_LBUTTONDBLCLK: {
	//	OnMouseDoubleClick(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
	//	break;
	//}
	//case WM_RBUTTONDBLCLK: {
	//	OnMouseDoubleClick(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
	//	break;
	//}
	case WM_LBUTTONDOWN:
	{
		/*	char buff[256]{ 0 };
			sprintf_s(buff, "X:%d Y:%d LParam:%d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), lParam);
			OutputDebugStringA(buff);*/
		OnMouseDown(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_RBUTTONDOWN:
	{
		OnMouseDown(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_LBUTTONUP:
	{
		OnMouseUp(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_RBUTTONUP:
	{
		OnMouseUp(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_MOUSELEAVE:
	{
		_mouseIn = false;
		OnMouseLeave();
		break;
	}

	case WM_MOUSEWHEEL:
	{
		auto fwKeys = GET_KEYSTATE_WPARAM(wParam);
		auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		auto xPos = GET_X_LPARAM(lParam);
		auto yPos = GET_Y_LPARAM(lParam);
		OnMouseWheel(zDelta, { xPos,yPos });
		break;
	}
	case WM_TIMER: {
		IControl* ctl = (IControl*)wParam;
		ctl->OnTimer();
		break;
	}
	default:
	{
		if (uMsg >= (WM_USER + 0x04) && uMsg <= (WM_USER + 0x0c)) { //
			MouseEventArgs* args = (MouseEventArgs*)lParam;
			return OnNotify((Control*)(wParam), *args);
		}
		break;
	}
	}
	return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
}

void Window::OnPaint(HDC winHDC, const Rect& rePaintRect)
{
	//StopWatch stopWatch;
	EBitmap memBitmap(GetClientRect().Width, GetClientRect().Height, 24);//
	Painter pt(memBitmap.GetHDC());
	PaintEventArgs args(pt);
	args.InvalidRectangle = rePaintRect;
	args.HWnd = _hWnd;
	_layout->OnEvent(Event::OnPaint, &args);//
	::BitBlt(winHDC, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, memBitmap.GetHDC(), rePaintRect.X, rePaintRect.Y, SRCCOPY);//
	//time_t ms = stopWatch.ElapsedMilliseconds();//
	//CHAR buf[256]{ 0 };
	//sprintf_s(buf, "GDIPaint %d %d   %d ms \n", rePaintRect.Width, rePaintRect.Height, (int)ms);
	//OutputDebugStringA(buf);
}

bool Window::IsInWindow(Control& pControl, Control& it) {
	Rect& winClientRect = GetClientRect();
	auto rect = it.GetRect();//

	if (rect.IsEmptyArea()) {
		return false;
	}
	auto clientRect = it.GetClientRect();//
	if (!winClientRect.IntersectsWith(clientRect)) {
		return false;
	}
	if (!pControl.GetClientRect().IntersectsWith(clientRect)) {
		return false;
	}
	return true;
}

Control* Window::FindControl(const Point& clientPoint, Point& outPoint) {
	outPoint = clientPoint;
	Control* outCtl = _layout;
loop:
	Control* scrollBar = outCtl->ScrollBar;
	if (scrollBar && scrollBar->GetClientRect().Contains(clientPoint)) {
		if (scrollBar->Visible) {
			auto barRect = scrollBar->GetClientRect();
			outPoint.X = clientPoint.X - barRect.X;
			outPoint.Y = clientPoint.Y - barRect.Y;
			outCtl = scrollBar;
			return outCtl;
		}
	}
	std::list<Control*>* pTemp;
	if (outCtl->VisibleControls.size() > 0) {
		pTemp = &outCtl->VisibleControls;
	}
	else {
		pTemp = (Controls*)outCtl->GetControls();
	}

	for (auto i = pTemp->rbegin(); i != pTemp->rend(); i++) {
		Control& it = **i;
		if (!it.Visible) {
			continue;
		}

		if (!IsInWindow(*outCtl, it)) {
			continue;
		}
		if (it.GetClientRect().Contains(clientPoint)) {
			if (dynamic_cast<Spacer*>(&it)) {
				return outCtl;
			}
			outCtl = &it;
			auto ctlRect = it.GetClientRect();
			outPoint.X = clientPoint.X - ctlRect.X;
			outPoint.Y = clientPoint.Y - ctlRect.Y;
			goto loop;
		}
	}
	return outCtl;
}

void Window::OnMouseMove(const Point& point)
{

	if (_focusControl && _mouseDown) {
		auto ctlRect = _focusControl->GetClientRect();
		MouseEventArgs args;
		args.Location = { point.X - ctlRect.X ,point.Y - ctlRect.Y };
		args.EventType = Event::OnMouseMove;
		_focusControl->OnMouseEvent(args);
		return;
	}

	Point relativePoint;
	Control* outCtl = FindControl(point, relativePoint);
	MouseEventArgs args;

	if (_focusControl && outCtl != _focusControl) {
		args.EventType = Event::OnMouseLeave;
		_focusControl->OnMouseEvent(args);
	}
	args.EventType = Event::OnMouseMove;
	args.Location = relativePoint;
	outCtl->OnMouseEvent(args);
	_focusControl = outCtl;

}
void Window::OnMouseLeave()
{
	if (_focusControl) {
		MouseEventArgs args;
		args.EventType = Event::OnMouseLeave;
		_focusControl->OnMouseEvent(args);
	}
	_focusControl = NULL;
	_mouseDown = false;
}


void Window::OnMouseWheel(short zDelta, const Point& point)
{
	if (_focusControl == NULL) return;
	ScrollBar* vBar = NULL;
	if (_focusControl->ScrollBar) {
		vBar = dynamic_cast<ScrollBar*>(_focusControl->ScrollBar);
	}
	Control* pControl = _focusControl;
	while (vBar == NULL && pControl)
	{
		if (pControl->ScrollBar) {
			vBar = dynamic_cast<ScrollBar*>(pControl->ScrollBar);
			break;
		}
		pControl = pControl->Parent;
	}
	if (vBar) {
		MouseEventArgs args;
		args.Delta = zDelta;
		args.Location = point;
		args.EventType = Event::OnMouseWheel;
		vBar->OnMouseEvent(args);
	}
}
void Window::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
{
	Control* outCtl = _lastDownCtl;
	MouseEventArgs args;
	args.Button = mbtn;
	args.Location = *_mouseDbClick;
	args.EventType = Event::OnMouseDoubleClick;
	outCtl->OnMouseEvent(args);
}

void Window::OnMouseDown(MouseButton mbtn, const Point& point)
{
	::SetFocus(_hWnd);
	_mouseDown = true;
	::SetCapture(_hWnd);

	Point relativePoint;
	Control* outCtl = FindControl(point, relativePoint);

	MouseEventArgs args;
	args.Button = mbtn;
	args.Location = relativePoint;
	args.EventType = Event::OnMouseDown;
	outCtl->OnMouseEvent(args);

	_inputControl = _focusControl;
	{ //做双击消息处理
		auto _time = std::chrono::system_clock::now();
		auto diff = _time - _lastDownTime;
		auto timeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		_lastDownTime = _time;
		if (timeOffset < 200) {//200毫秒之内单机两次算双击消息
			_lastDownCtl = outCtl;
			_lastDownTime = std::chrono::system_clock::from_time_t(0);
			_mouseDbClick = &relativePoint;
			OnMouseDoubleClick(mbtn, point);
		}
	}

	if (_focusControl && _focusControl != outCtl) {
		args.EventType = Event::OnMouseLeave;
		_focusControl->OnMouseEvent(args);
	}
	_focusControl = outCtl;



}

void Window::MoveWindow() {
	::ReleaseCapture();
	SendMessage(_hWnd, 161, 2, NULL);
	SendMessage(_hWnd, 0x0202, 0, NULL);
}

void Window::OnMouseUp(MouseButton mbtn, const Point& point)
{
	_mouseDown = false;
	::ReleaseCapture();
	if (_focusControl) {
		auto ctlRect = _focusControl->GetClientRect();
		MouseEventArgs args;
		args.Button = mbtn;
		args.Location = { point.X - ctlRect.X,point.Y - ctlRect.Y };
		args.EventType = Event::OnMouseUp;
		_focusControl->OnMouseEvent(args);
		if (_focusControl && !ctlRect.Contains(point))
		{
			args.EventType = Event::OnMouseLeave;
			_focusControl->OnMouseEvent(args);
		}
	}
}
void Window::OnSize(const Size& sz)
{
	*((Rect*)(&_layout->ClipRect)) = this->GetClientRect();//
	_layout->SetRect(this->GetClientRect(), true);
}

void Window::OnRect(const Rect& rect)
{
}
void Window::OnClose(bool& Cancel)
{
}
void Window::OnDestroy()
{
}
void Window::OnLoad()
{

}

void Window::OnKeyDown(WPARAM wParam) {

	if (_inputControl) { //
		_inputControl->OnKeyDown(wParam);
		return;
	}
	if (_focusControl) {//
		_focusControl->OnKeyDown(wParam);
	}
}
void Window::OnChar(WPARAM wParam, LPARAM lParam)
{
	if (_inputControl) { //
		_inputControl->OnChar(wParam, lParam);
		return;
	}
	if (_focusControl) {//
		_focusControl->OnChar(wParam, lParam);
	}
}

void Window::OnMove(const Point& point) {

}

bool Window::OnNotify(Control* sender,const EventArgs& args) {
	if (args.EventType == Event::OnMouseDown) {
		if (sender->Action == ControlAction::MoveWindow || sender == _layout) {
			MoveWindow();
			return true;
		}
		if (sender->Action == ControlAction::Mini) {
			::ShowWindow(_hWnd, SW_MINIMIZE);
			return true;
		}
		if (sender->Action == ControlAction::Max) {
			if (!IsZoomed(_hWnd)) {
				::ShowWindow(_hWnd, SW_MAXIMIZE);
			}
			else {
				::ShowWindow(_hWnd, SW_SHOWNORMAL);
			}
			return true;
		}
		if (sender->Action == ControlAction::Close) {
			MouseEventArgs args;
			args.EventType = Event::OnMouseLeave;
			sender->OnMouseEvent(args);
			this->Close();
			return true;
		}
	}
	return false;
}