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
void Window::SetUserMessageProc(const UserMessageProc & userMessageProc)
{
	_userMessageProc = userMessageProc;
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
void Window::SetText(const EString&text)
{
	::SetWindowTextW(_hWnd, text.c_str());
}
void Window::ReSize(const Size&size) {
	RECT rect;
	::GetWindowRect(_hWnd, &rect);
	::MoveWindow(_hWnd, rect.left, rect.right, size.Width, size.Height, TRUE);
}
void Window::SetIcon(short id)
{
	SetIcon(::LoadIcon(::GetModuleHandle(0), MAKEINTRESOURCE(id)));//加载图标
}
void Window::SetIcon(HICON icon)
{
	::SendMessage(_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
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
	//if (!_layout->Style.BackgroundColor.valid) {
	//	_layout->Style.BackgroundColor = Color::White;
	//}
	if (!_layout->Style.ForeColor.valid) {
		_layout->Style.ForeColor = Color::Black;
	}
	_layout->ParentWid = _hWnd;
	_layout->SetRect(this->GetClientRect());
}
void Window::Close() {
	::SendMessage(_hWnd, WM_CLOSE, 0, 0);
}
void Window::Show(int cmdShow)
{
	ASSERT(_layout);
	::ShowWindow(_hWnd, cmdShow);
	//// 处理多显示器模式下屏幕居中
	//MONITORINFO oMonitor = {};
	//oMonitor.cbSize = sizeof(oMonitor);
	//::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	//rcArea = oMonitor.rcWork;
}

void Window::ShowModal(bool wait)
{
	auto p_hwnd = ::GetWindowOwner(_hWnd);
	ASSERT(::IsWindow(p_hwnd));
	::EnableWindow(p_hwnd, FALSE);
	this->Show();
	::SetFocus(_hWnd);
	if (wait) {//如果需要阻塞
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (!::IsWindow(_hWnd) || msg.message == WM_QUIT) {
				break;
			}
		}
		if (msg.message == WM_QUIT) {//后面的窗口也不再进行消息循环
			::PostQuitMessage(msg.wParam);
		}
	}
	::EnableWindow(p_hwnd, TRUE);
	::SetFocus(p_hwnd);
}

void Window::Hide() {
	::ShowWindow(_hWnd, SW_HIDE);
}
bool Window::IsVisible() {
	return ::IsWindowVisible(_hWnd) ? true : false;
}
void Window::SetVisible(bool flag) {
	if (flag) {
		::ShowWindow(_hWnd, SW_SHOW);
		::SetForegroundWindow(_hWnd);
	}
	else {
		Hide();
	}
}
void Window::EmptyControl(Controls*controls) {
	for (auto it : *controls) {
		if (_focusControl == it) {
			_focusControl = NULL;
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
		Control *win32Control = (Control*)UI_GetUserData((HWND)lParam);
		EditWnd *win32Edit = dynamic_cast<EditWnd*>(win32Control);
		if (win32Edit) {
			return win32Edit->WndProc(uMsg, wParam, lParam);
		}
	}

	switch (uMsg)
	{
	


	case  WM_IME_STARTCOMPOSITION://处理输入法坐标
	{
		HIMC hIMC = ImmGetContext(_hWnd);
		COMPOSITIONFORM cpf;
		cpf.dwStyle = CFS_POINT;
		int x = 0;
		int y = 0;
		if (_inputControl) {
			auto &ime = *_inputControl;
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
	case WM_ERASEBKGND: {//不允许绘制背景
		return TRUE;
	}
	case WM_CONTROL_REFRESH: {
		Control *ctl = (Control*)wParam;
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
		PAINTSTRUCT pst;
		BeginPaint(_hWnd, &pst);
		RECT &r = pst.rcPaint;
		Rect paintRect{ r.left,r.top,r.right - r.left, r.bottom - r.top };
		OnPaint(pst.hdc, paintRect);
		EndPaint(_hWnd, &pst);
		if (!_load) {
			OnLoad();
			_load = true;
		}
		break;
	}
	case WM_CONTROL_DELETE:
	{
		//当窗口下的子控件删除的时候将当前鼠标操作的控件清空
		Control *delControl = (Control*)wParam;
		EmptyControl((Controls*)delControl->GetControls());
		return TRUE;
	}
	case WM_WINDOWPOSCHANGED: {
		bool rePaint = false;
		WINDOWPOS *wPos = (WINDOWPOS *)(void*)lParam;
		int flag = SWP_NOCOPYBITS;//
		if ((wPos->flags & flag) == flag) {
			//丢弃客户区的全部内容。如果未指定此标志，则在调整窗口大小或重新定位后，将保存客户区的有效内容并将其复制回客户区。
			rePaint = true;
		}
		RECT rect;
		::GetClientRect(_hWnd, &rect);
		Point point{ rect.left,rect.top };
		Size size{ rect.right - rect.left,rect.bottom - rect.top };
		_rectClient = { point,size };
		if (_rectClient.IsEmptyArea()) {
			_rect = { 0,0,0,0 };
			return TRUE;//不在发送 WM_MOVE 和 WM_SIZE 消息
		}
		_rect = { wPos->x,wPos->y,wPos->cx,wPos->cy };
		if (!_lastSize.Equals(size)) {
			_lastSize = size;//当窗口大小和上次不一样的时候
			OnSize(size);
		}
		else if (rePaint) {
			//对于layeredwindow来说 就算窗口大小和上次一样也要重新绘制,有SWP_NOCOPYBITS就需要重新绘制
			Debug::Log("SWP_NOCOPYBITS重绘!");
			OnSize(size);
		}
		if (!_lastPoint.Equals(point)) {
			_lastPoint = point;
			OnMove(point);
		}
		OnRect(_rect);
		return TRUE;//不在发送 WM_MOVE 和 WM_SIZE 消息
	}
	case WM_CLOSE:
	{
		if (OnClose()) {
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
	//鼠标移动
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

	//鼠标双击 左键
	case WM_LBUTTONDBLCLK: {
		OnMouseDoubleClick(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
						   //鼠标双击 右键
	case WM_RBUTTONDBLCLK: {
		OnMouseDoubleClick(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
						   //鼠标按下 左键
	case WM_LBUTTONDOWN:
	{

		OnMouseDown(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	//鼠标按下 右键
	case WM_RBUTTONDOWN:
	{
		OnMouseDown(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	//鼠标弹起 左键
	case WM_LBUTTONUP:
	{
		OnMouseUp(MouseButton::Left, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	//鼠标弹起 右键
	case WM_RBUTTONUP:
	{
		OnMouseUp(MouseButton::Right, { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}
	//鼠标移出
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
		//使用以下代码获取水平和垂直位置：
		auto xPos = GET_X_LPARAM(lParam);
		auto yPos = GET_Y_LPARAM(lParam);
		OnMouseWheel(zDelta, { xPos,yPos });
		break;
	}
	case WM_TIMER: {
		Control *ctl = (Control*)wParam;
		ctl->OnTimer();
		break;
	}
	default:
	{
		if (uMsg >= (WM_USER + 0x04) && uMsg <= (WM_USER + 0x0c)) { //鼠标范围消息
			MouseEventArgs *args = (MouseEventArgs*)lParam;
			return OnNotify((Control*)(wParam), args);
		}
		if (uMsg > WM_UIMESSAGE && _userMessageProc) {
			_userMessageProc(uMsg, wParam, lParam);
			return true;
		}
		break;
	}
	}
	return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
}

void Window::OnPaint(HDC winHDC, const Rect&rePaintRect)
{
	StopWatch stopWatch;//计算耗时
	EBitmap memBitmap(GetClientRect().Width, GetClientRect().Height, 24);//双缓冲位图
	Painter pt(memBitmap.GetHDC());
	PaintEventArgs args(pt);
	args.InvalidRectangle = rePaintRect;
	args.HWnd = _hWnd;
	_layout->OnEvent(Event::OnPaint, &args);//给布局器绘制内容
	::BitBlt(winHDC, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, memBitmap.GetHDC(), rePaintRect.X, rePaintRect.Y, SRCCOPY);//贴图
	time_t ms = stopWatch.ElapsedMilliseconds();//获取耗时结果
	CHAR buf[256]{ 0 };
	sprintf_s(buf, "GDIPaint %d %d   %d ms \n", rePaintRect.Width, rePaintRect.Height, (int)ms);
	OutputDebugStringA(buf);
}

bool Window::IsInWindow(Control&pControl, Control&it) {
	Rect &winClientRect = GetClientRect();
	auto rect = it.GetRect();//获取子控件矩形

	if (rect.IsEmptyArea()) {
		return false;
	}
	auto clientRect = it.GetClientRect();//获取基于父窗口的最表
	if (!winClientRect.IntersectsWith(clientRect)) {
		return false;
	}
	if (!pControl.GetClientRect().IntersectsWith(clientRect)) {
		return false;
	}
	return true;
}

Control*  Window::FindControl(const Point & clientPoint, Point&outPoint) {
	outPoint = clientPoint;
	Control *outCtl = _layout;
loop:
	//滚动条优先
	Control *scrollBar = outCtl->ScrollBar;
	if (scrollBar && scrollBar->GetClientRect().Contains(clientPoint)) {
		if (scrollBar->Visible) {
			auto &barRect = scrollBar->GetClientRect();
			outPoint.X = clientPoint.X - barRect.X;
			outPoint.Y = clientPoint.Y - barRect.Y;
			outCtl = scrollBar;
			return outCtl;
		}
	}
	std::list<Control*> *pTemp;
	if (outCtl->VisibleControls.size() > 0) { //如果有控件收集了可见子控件 则只对可见控件进行寻找坐标
		pTemp = &outCtl->VisibleControls;
	}
	else {
		pTemp = (Controls*)outCtl->GetControls();
	}
	//对可见子控件进行寻找
	for (auto i = pTemp->rbegin(); i != pTemp->rend(); i++) {
		Control &it = **i;
		if (!it.Visible) {
			continue;
		}
		//判断是不是在窗口内可见
		if (!IsInWindow(*outCtl, it)) {
			continue;
		}
		if (it.GetClientRect().Contains(clientPoint)) {
			Spacer*spcaer = DynamicCast((&it), Spacer, ControlType::ControlSpacer);
			if (spcaer) {//如果该控件是弹簧则不接收任何鼠标消息
				return outCtl;
			}
			outCtl = &it;
			auto &ctlRect = it.GetClientRect();
			outPoint.X = clientPoint.X - ctlRect.X;
			outPoint.Y = clientPoint.Y - ctlRect.Y;
			goto loop;
		}
	}
	return outCtl;
}

void Window::OnMouseMove(const Point & point)
{

	if (_focusControl && _mouseDown) { //如果鼠标按下某个控件 则相应按下鼠标的移动消息
		auto &ctlRect = _focusControl->GetClientRect();
		MouseEventArgs args;
		args.Location = { point.X - ctlRect.X ,point.Y - ctlRect.Y };
		args.EventType = Event::OnMouseMove;
		_focusControl->OnMouseEvent(args);//触发移动消息
		return;
	}

	Point relativePoint;
	Control *outCtl = FindControl(point, relativePoint);
	MouseEventArgs args;

	if (_focusControl && outCtl != _focusControl) {//让之前的控件先发送移出消息
		args.EventType = Event::OnMouseLeave;
		_focusControl->OnMouseEvent(args);//如果当前控件不是上一个具有焦点的控件 触发上一个控件的移出消息
	}
	args.EventType = Event::OnMouseMove;
	args.Location = relativePoint;
	outCtl->OnMouseEvent(args);//最后开始出发当前控件的移动消息
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


void Window::OnMouseWheel(short zDelta, const Point & point)
{
	if (_focusControl == NULL) return;
	ScrollBar * vBar = NULL;
	if (_focusControl->ScrollBar) {
		vBar = DynamicCast(_focusControl->ScrollBar, ScrollBar, ControlType::ControlScrollBar);
	}
	Control *pControl = _focusControl;
	while (vBar == NULL && pControl)
	{
		if (pControl->ScrollBar) {
			vBar = DynamicCast(pControl->ScrollBar, ScrollBar, ControlType::ControlScrollBar);
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

		//滚动条滚动之后仍然要要鼠标悬浮在控件上
	/*	POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(Hwnd(), &pt);
		Debug::Log("%d %d", pt.x, pt.y);
		OnMouseMove({ pt.x,pt.y });*/
	}

}
void Window::OnMouseDoubleClick(MouseButton mbtn, const Point & point)
{
	Point relativePoint;
	Control *outCtl = FindControl(point, relativePoint);
	MouseEventArgs args;
	args.Button = mbtn;
	args.Location = relativePoint;
	args.EventType = Event::OnMouseDoubleClick;
	outCtl->OnMouseEvent(args);
}

void Window::OnMouseDown(MouseButton mbtn, const Point & point)
{
	_mouseDown = true;
	::SetCapture(_hWnd);

	Point relativePoint;
	Control *outCtl = FindControl(point, relativePoint);

	MouseEventArgs args;
	args.Button = mbtn;
	args.Location = relativePoint;
	args.EventType = Event::OnMouseDown;
	outCtl->OnMouseEvent(args);

	_inputControl = _focusControl;

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

void Window::OnMouseUp(MouseButton mbtn, const Point & point)
{
	_mouseDown = false;
	::ReleaseCapture();
	if (_focusControl) {
		auto &ctlRect = _focusControl->GetClientRect();
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
void Window::OnSize(const Size & sz)
{
	*((Rect*)(&_layout->ClipRect)) = this->GetClientRect();//布局要先确定自己的裁剪区域是和窗口一样大小
	//先让区域有效
	//RECT rc{ _layout->ClipRect.X,_layout->ClipRect.Y,_layout->ClipRect.GetRight(),_layout->ClipRect.GetBottom() };
	//::ValidateRect(_hWnd, &rc);
	_layout->SetRect(this->GetClientRect(), true);//让布局器重绘 整个窗口都会重绘
}

void Window::OnRect(const Rect & rect)
{
}
bool Window::OnClose()
{
	return true;
}
void Window::OnDestroy()
{
}
void Window::OnLoad()
{

}

void Window::OnKeyDown(WPARAM wParam) {

	if (_inputControl) { //输入框优先级高
		_inputControl->OnKeyDown(wParam);
		return;
	}
	if (_focusControl) {//然后再响应其他控件的按下事件
		_focusControl->OnKeyDown(wParam);
	}
}
void Window::OnChar(WPARAM wParam, LPARAM lParam)
{
	if (_inputControl) { //输入框优先级高
		_inputControl->OnChar(wParam, lParam);
		return;
	}
	if (_focusControl) {//然后再响应其他控件的按下事件
		_focusControl->OnChar(wParam, lParam);
	}
}

void Window::OnMove(const Point &point) {

}

bool Window::OnNotify(Control *sender, EventArgs *args) {
	if (args->EventType == Event::OnMouseDown) {
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