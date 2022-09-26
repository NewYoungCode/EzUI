#include "Window.h"
#include "TabLayout.h"
namespace EzUI {

	Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
	{
		MONITORINFO monitor{ 0 };
		monitor.cbSize = sizeof(MONITORINFO);
		::GetMonitorInfo(::MonitorFromWindow(_hWnd, MONITOR_DEFAULTTONEAREST), &monitor);

		const RECT& rcWork = monitor.rcWork;
		int sw = rcWork.right - rcWork.left;//当前工作区域的宽
		int sh = rcWork.bottom - rcWork.top;//当前工作区域的高
		_rect.X = rcWork.left + (sw - width) / 2;//保证左右居中
		_rect.Y = rcWork.top + (sh - height) / 2;//保证上下居中
		_rect.Width = width;
		_rect.Height = height;

		_hWnd = ::CreateWindowEx(ExStyle | WS_EX_ACCEPTFILES, UI_CLASSNAME, UI_CLASSNAME, dStyle,
			_rect.X, _rect.Y, width, height, owner, NULL, GetModuleHandle(NULL), NULL);

		InitData(ExStyle);//设置基本数据
	}

	Window::~Window()
	{
		if (::IsWindow(_hWndTip)) {
			::DestroyWindow(_hWndTip);
		}
		if (::IsWindow(_hWnd)) {
			::DestroyWindow(_hWnd);
		}
	}

	Control* Window::FindControl(const EString& objectName)
	{
		return this->MainLayout->FindControl(objectName);
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
	void Window::SetLayout(EzUI::Layout* layout) {
		ASSERT(layout);
		MainLayout = layout;
		if (MainLayout->Style.FontFamily.empty()) {
			MainLayout->Style.FontFamily = TEXT("Microsoft YaHei");
		}
		if (!MainLayout->Style.FontSize.valid) {
			MainLayout->Style.FontSize = 9;
		}
		if (!MainLayout->Style.ForeColor.valid) {
			MainLayout->Style.ForeColor = Color::Black;
		}
		MainLayout->PublicData = &PublicData;
		MainLayout->SetRect(this->GetClientRect());
	}
	void Window::Close(int code) {
		_closeCode = code;
		::SendMessageW(_hWnd, WM_CLOSE, 0, 0);
	}
	void Window::Show(int cmdShow)
	{
		ASSERT(MainLayout);
		::ShowWindow(_hWnd, cmdShow);
	}
	int Window::ShowModal(bool wait)
	{
		_OwnerHwnd = ::GetWindowOwner(_hWnd);
		ASSERT(::IsWindow(_OwnerHwnd));
		Show();
		::EnableWindow(_OwnerHwnd, FALSE);
		if (wait) {//
			MSG msg{ 0 };
			while (::IsWindow(_hWnd) && ::GetMessage(&msg, NULL, 0, 0) && msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) {//
				::PostQuitMessage(msg.wParam);
				return _closeCode;
			}
		}
		::SetForegroundWindow(_OwnerHwnd);
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

	//#pragma comment(lib,"odbc32.lib")
	//#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"imm32.lib")
	LRESULT HandleStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		return 1;
	}
	bool _Has(UINT uMsg, UINT msg) {
		if (uMsg == msg) return true;
		return false;
	}
#define HAS(arg) _Has(uMsg,arg)

	LRESULT  Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//if (_ChildModalWnd) {
		//	if (uMsg == WM_SETFOCUS) {
		//		FLASHWINFO info;
		//		info.cbSize = sizeof(FLASHWINFO);
		//		info.hwnd = _ChildModalWnd;
		//		info.dwFlags = FLASHW_ALL;
		//		info.dwTimeout = 100;
		//		info.uCount = 5;
		//		::FlashWindowEx(&info);
		//		::SetFocus(_ChildModalWnd);
		//		::MessageBeep(MB_ICONWARNING);//发出咚咚咚的声音
		//	}
		//	if (uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE) {
		//		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
		//	}
		//} //已经解决owner闪烁的问题 不需要再拦截这些信息来实现 阻塞对话框

		if (WM_COMMAND == uMsg) {

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
		case WM_NOTIFY: {
			LPNMTTDISPINFO lpnmttdi = (LPNMTTDISPINFO)lParam;
			if (lpnmttdi->hdr.code == TTN_GETDISPINFO) {
				//拦截冒泡提示文字
			}
			break;
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
			OnRect(_rect);
			if (!_lastSize.Equals(size)) {
				_lastSize = size;
				OnSize(size);
			}
			else if (rePaint) {
				//Debug::Log("SWP_NOCOPYBITS!");
				OnSize(size);
			}
			if (!_lastPoint.Equals(point)) {
				_lastPoint = point;
				OnMove(point);
			}
			return TRUE;
		}
		case WM_CLOSE:
		{
			bool Cancel = false;
			OnClose(Cancel);
			if (!Cancel) {
				if (_OwnerHwnd) {
					::EnableWindow(_OwnerHwnd, TRUE);
					_OwnerHwnd = NULL;
				}
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
			if (wParam == VK_F11) {
				PublicData.Debug = !PublicData.Debug;
				MainLayout->Invalidate();
			}
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
			//给hwndTip发送消息告诉现在移动到什么位置了
			SendMessage(_hWndTip, TTM_TRACKPOSITION, 0, lParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
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

			break;
		}
		}
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

	void Window::OnPaint(HDC winHDC, const Rect& rePaintRect)
	{

#ifdef COUNT_ONPAINT
		StopWatch sw;
#endif // COUNT_ONPAINT

#if USED_GDIPLUS
		EBitmap memBitmap(GetClientRect().Width, GetClientRect().Height);//
		Painter pt(memBitmap.GetDC());
		PaintEventArgs args(pt);
		args.DC = memBitmap.GetDC();
		args.PublicData = &PublicData;
		args.InvalidRectangle = rePaintRect;
		MainLayout->Rending(args);//
#ifdef DEBUGPAINT
		if (PublicData.Debug) {
			pt.DrawRectangle(rePaintRect, Color::Red);
		}
#endif
		::BitBlt(winHDC, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, memBitmap.GetDC(), rePaintRect.X, rePaintRect.Y, SRCCOPY);//
#endif
#if USED_Direct2D
		Painter pt(winHDC, GetClientRect().Width, GetClientRect().Height);
		PaintEventArgs args(pt);
		args.DC = winHDC;
		args.PublicData = &PublicData;
		args.InvalidRectangle = rePaintRect;
		MainLayout->Rending(args);//
#ifdef DEBUGPAINT
		if (PublicData.Debug) {
			pt.DrawRectangle(rePaintRect, Color::Red);
		}
#endif

#endif

#ifdef COUNT_ONPAINT
		char buf[100]{ 0 };
		sprintf_s(buf, "OnPaint (%d,%d,%d,%d) %dms \n", rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, sw.ElapsedMilliseconds());
		OutputDebugStringA(buf);
#endif // COUNT_ONPAINT

	}

	void Window::InitData(const DWORD& ExStyle)
	{
		PublicData.HANDLE = _hWnd;
		PublicData.Window = this;

		if ((ExStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			PublicData.InvalidateRect = [=](void* _rect)->void {
				RECT r = ((Rect*)_rect)->WinRECT();
				::InvalidateRect(_hWnd, &r, FALSE);
			};
			PublicData.UpdateWindow = [=]()->void {
				::UpdateWindow(_hWnd);
			};
		}

		PublicData.RemoveControl = [=](Control* delControl)->void {
			/*	char buf[100]{ 0 };
				sprintf_s(buf, "RemoveControl %p\n", delControl);
				OutputDebugStringA(buf);*/
			if (_focusControl == delControl) {
				_focusControl->Trigger(Event::OnMouseLeave);
				_focusControl = NULL;
				return;
			}
			if (_inputControl == delControl) {
				_inputControl->OnKillFocus();
				_inputControl = NULL;
				return;
			}
		};

		PublicData.Notify = [=](UINT uMsg, WPARAM wParam, LPARAM lParam)->bool {
			if (uMsg >= (WM_USER + 0x04) && uMsg <= (WM_USER + 0x0c)) { //
				MouseEventArgs* args = (MouseEventArgs*)lParam;
				return OnNotify((Control*)(wParam), *args);
			}
			return FALSE;
		};
		PublicData.SetTips = [=](Control* ctl, const std::wstring& text)->void {
			TOOLINFO	tti{ 0 };
			tti.cbSize = sizeof(TOOLINFO);
			tti.uFlags = TTF_SUBCLASS;
			tti.hwnd = _hWnd;
			tti.rect = ctl->ClipRect.WinRECT();
			tti.uId = (UINT_PTR)ctl;
			tti.lpszText = (LPWSTR)text.c_str();
			//添加一个tips信息
			SendMessage(_hWndTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
		};
		PublicData.DelTips = [=](Control* ctl)->void {
			TOOLINFO	tti{ 0 };
			tti.cbSize = sizeof(TOOLINFO);
			tti.hwnd = _hWnd;
			tti.uId = (UINT_PTR)ctl;
			//移除
			SendMessage(_hWndTip, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
		};

		PublicData.GetCursor = [=]()->Cursor {
			return (Cursor)::GetClassLongPtr(_hWnd, GCL_HCURSOR);//获取鼠标状态
		};

		PublicData.SetCursor = [=](Cursor cursor)->void {
			if (((ULONG)Cursor::ALL & (ULONG)cursor) == (ULONG)cursor) {//判断在不在预设中
				::SetClassLongPtr(_hWnd, GCL_HCURSOR, (UINT_PTR)::LoadCursor(NULL, (LPTSTR)(cursor)));//设置鼠标样式
			}
			else {
				::SetClassLongPtr(_hWnd, GCL_HCURSOR, (UINT_PTR)cursor);//
			}
		};

		//创建冒泡提示窗口
		_hWndTip = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS,
			NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			_hWnd,
			NULL,
			::GetModuleHandle(NULL),
			NULL
		);

		UI_SetUserData(_hWnd, &PublicData);
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
		Control* outCtl = MainLayout;
	UI_Loop:
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

		Controls* pTemp;
		if (outCtl->VisibleControls.size() > 0) {
			pTemp = &outCtl->VisibleControls;
		}
		else {
			pTemp = &(outCtl->GetControls());
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
				goto UI_Loop;
			}
		}
		return outCtl;
	}

	void Window::OnMouseMove(const Point& point)
	{
		if (_focusControl && _mouseDown) {
			auto ctlRect = _focusControl->GetClientRect();
			MouseEventArgs args(Event::OnMouseMove, { point.X - ctlRect.X ,point.Y - ctlRect.Y });
			_focusControl->Trigger(args);
			return;
		}

		Point relativePoint;
		Control* outCtl = FindControl(point, relativePoint);
		MouseEventArgs args;

		if (_focusControl && outCtl != _focusControl) {
			args.EventType = Event::OnMouseLeave;
			_focusControl->Trigger(args);
		}
		args.EventType = Event::OnMouseMove;
		args.Location = relativePoint;
		outCtl->Trigger(args);
		_focusControl = outCtl;

	}
	void Window::OnMouseLeave()
	{
		if (_focusControl) {
			MouseEventArgs args(Event::OnMouseLeave);
			_focusControl->Trigger(args);
		}
		_focusControl = NULL;
		_mouseDown = false;
	}


	void Window::OnMouseWheel(short zDelta, const Point& point)
	{
		if (_focusControl == NULL) return;
		ScrollBar* scrollBar = NULL;
		if (_focusControl->ScrollBar) {
			scrollBar = dynamic_cast<ScrollBar*>(_focusControl->ScrollBar);
		}
		Control* pControl = _focusControl;
		while (scrollBar == NULL && pControl)
		{
			if (pControl->ScrollBar) {
				scrollBar = dynamic_cast<ScrollBar*>(pControl->ScrollBar);
				break;
			}
			pControl = pControl->Parent;
		}
		if (scrollBar) {
			MouseEventArgs args;
			args.Delta = zDelta;
			args.Location = point;
			args.EventType = Event::OnMouseWheel;
			scrollBar->Trigger(args);
			PublicData.UpdateWindow();
			POINT p1{ 0 };
			::GetCursorPos(&p1);
			::ScreenToClient(_hWnd, &p1);
			OnMouseMove({ p1.x, p1.y });
		}
	}
	void Window::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
	{
		Control* outCtl = _lastDownCtl;
		MouseEventArgs args;
		args.Button = mbtn;
		args.Location = *_mouseDbClick;
		args.EventType = Event::OnMouseDoubleClick;
		outCtl->Trigger(args);
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
		outCtl->Trigger(args);

		{ //做双击消息处理
			auto _time = std::chrono::system_clock::now();
			auto diff = _time - _lastDownTime;
			auto timeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
			_lastDownTime = _time;
			if (timeOffset < 300) {//300毫秒之内单机两次算双击消息
				_lastDownCtl = outCtl;
				_lastDownTime = std::chrono::system_clock::from_time_t(0);
				_mouseDbClick = &relativePoint;
				OnMouseDoubleClick(mbtn, point);
			}
		}
		if (_focusControl && _focusControl != outCtl) {
			args.EventType = Event::OnMouseLeave;
			_focusControl->Trigger(args);
		}
		_focusControl = outCtl;

		if (_inputControl && _inputControl != _focusControl) { //输入焦点更换
			_inputControl->OnKillFocus();//给上一个输入焦点触发失去焦点的事件
		}
		_inputControl = _focusControl;

	}

	void Window::MoveWindow() {
		::ReleaseCapture();//会导致avtiveStyle失效
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
			_focusControl->Trigger(args);
			if (_focusControl && !ctlRect.Contains(point))
			{
				args.EventType = Event::OnMouseLeave;
				_focusControl->Trigger(args);
			}
		}
	}
	void Window::OnSize(const Size& sz)
	{
#ifdef COUNT_ONSIZE
		StopWatch sw;
#endif 
		* ((Rect*)(&MainLayout->ClipRect)) = this->GetClientRect();//
		MainLayout->SetRect(this->GetClientRect(), true);
#ifdef COUNT_ONSIZE
		char buf[50]{ 0 };
		sprintf_s(buf, "OnSize (%d,%d) %dms\n", sz.Width, sz.Height, sw.ElapsedMilliseconds());
		OutputDebugStringA(buf);
#endif
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

	bool Window::OnNotify(Control* sender, const EventArgs& args) {
		if (args.EventType == Event::OnMouseDown) {
			if (sender->Action == ControlAction::MoveWindow || sender == MainLayout) {
				MoveWindow();
				return true;
			}
			if (sender->Action == ControlAction::Mini) {
				::ShowWindow(_hWnd, SW_MINIMIZE);
				return false;
			}
			if (sender->Action == ControlAction::Max) {
				if (!IsZoomed(_hWnd)) {
					::ShowWindow(_hWnd, SW_MAXIMIZE);
				}
				else {
					::ShowWindow(_hWnd, SW_SHOWNORMAL);
				}
				return false;
			}
			if (sender->Action == ControlAction::Close) {
				MouseEventArgs args;
				args.EventType = Event::OnMouseLeave;
				sender->Trigger(args);
				this->Close();
				return false;
			}
		}
		if (args.EventType == Event::OnMouseClick) {
			EString  ctlName = sender->GetAttribute("tablayout");
			if (!ctlName.empty()) {
				TabLayout* tabLayout = dynamic_cast<TabLayout*>(FindControl(ctlName));
				if (tabLayout) {
					tabLayout->SetPageIndex(sender->Index());
					tabLayout->Invalidate();
				}
			}
		}
		return false;
	}

};