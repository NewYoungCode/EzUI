#include "Window.h"
#include "TabLayout.h"

namespace EzUI {
#define _focusControl PublicData.FocusControl
#define _inputControl PublicData.InputControl

	float GetScale() {
		return 1.0f;

		HWND hWnd = GetDesktopWindow();
		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		// 获取监视器逻辑宽度与高度
		MONITORINFOEX miex;
		miex.cbSize = sizeof(miex);
		GetMonitorInfo(hMonitor, &miex);
		int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
		int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);
		// 获取监视器物理宽度与高度
		DEVMODE dm;
		dm.dmSize = sizeof(dm);
		dm.dmDriverExtra = 0;
		EnumDisplaySettings(miex.szDevice, ENUM_REGISTRY_SETTINGS, &dm);
		int cxPhysical = dm.dmPelsWidth;
		int cyPhysical = dm.dmPelsHeight;
		// 缩放比例计算  实际上使用任何一个即可
		double horzScale = ((double)cxPhysical / (double)cxLogical);
		double vertScale = ((double)cyPhysical / (double)cyLogical);
		//printf("%lf  %lf\n", horzScale, vertScale);
		return (float)horzScale;
	}


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

		_hWnd = ::CreateWindowExW(ExStyle | WS_EX_ACCEPTFILES, WindowClassName, WindowClassName, dStyle,
			_rect.X, _rect.Y, width, height, owner, NULL, GetModuleHandle(NULL), NULL);

		//EzUI::Scale = GetScale();
		InitData(ExStyle);//设置基本数据
	}

	Window::~Window()
	{
		if (::IsWindow(_hWnd)) {
			::DestroyWindow(_hWnd);
		}
	}

	Control* Window::FindControl(const EString& objectName)
	{
		if (!MainLayout) {
			return NULL;
		}
		return this->MainLayout->FindControl(objectName);
	}


	HWND& Window::Hwnd()
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
	void Window::SetSize(const Size& size) {
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
		::SendMessage(_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
	}
	void Window::SetLayout(EzUI::Control* layout) {
		ASSERT(layout);
		MainLayout = layout;
		if (MainLayout->Style.FontFamily.empty()) {
			MainLayout->Style.FontFamily = "Microsoft YaHei";
		}
		if (MainLayout->Style.FontSize == 0) {
			MainLayout->Style.FontSize = 12;
		}
		if (MainLayout->Style.ForeColor.GetValue() == 0) {
			MainLayout->Style.ForeColor = Color::Black;
		}
		MainLayout->PublicData = &PublicData;
		MainLayout->SetRect(this->GetClientRect());
	}
	void Window::Close(int code) {
		_closeCode = code;
		::SendMessage(_hWnd, WM_CLOSE, 0, 0);
	}
	void Window::Show(int cmdShow)
	{
		//ASSERT(MainLayout);
		::ShowWindow(_hWnd, cmdShow);
	}
	void Window::ShowMax() {
		Show(SW_MAXIMIZE);
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
		Show(SW_HIDE);
	}
	bool Window::IsVisible() {
		return ::IsWindowVisible(_hWnd) ? true : false;
	}
	void Window::SetVisible(bool flag) {
		if (flag) {
			Show(SW_RESTORE);
			::SetForegroundWindow(_hWnd);
		}
		else {
			Hide();
		}
	}

	void Window::Invalidate()
	{
		if (MainLayout) {
			MainLayout->Invalidate();
		}
	}
	void Window::Refresh()
	{
		if (MainLayout) {
			MainLayout->Refresh();
		}
	}

	LRESULT  Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (WM_COMMAND == uMsg) {
			//windows子控件消息
		}
		switch (uMsg)
		{
		case WM_GETMINMAXINFO:
		{
			MONITORINFO monitor{ 0 };
			monitor.cbSize = sizeof(MONITORINFO);
			::GetMonitorInfoW(::MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY), &monitor);
			RECT rcWork = monitor.rcWork;
			//是否为主要显示器
			if (monitor.dwFlags != MONITORINFOF_PRIMARY) {
				::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
			}
			//保证窗口在最大化的时候始终在工作区 不会遮挡任务栏
			MINMAXINFO* pMMInfo = (MINMAXINFO*)lParam;
			pMMInfo->ptMaxPosition.x = rcWork.left;
			pMMInfo->ptMaxPosition.y = rcWork.top;
			pMMInfo->ptMaxSize.x = rcWork.right - rcWork.left;
			pMMInfo->ptMaxSize.y = rcWork.bottom - rcWork.top;
			break;
		}
		case  WM_IME_STARTCOMPOSITION://
		{
			if (_inputControl) {
				HIMC hIMC = ImmGetContext(_hWnd);
				COMPOSITIONFORM cpf{ 0 };
				cpf.dwStyle = CFS_POINT;
				Control* input = _inputControl;
				Rect rect = input->GetCareRect();
				int	x = input->GetClientRect().X + rect.X;
				int y = input->GetClientRect().Y + rect.Y + rect.Height;
				if (y == _rectClient.Height) {
					y -= 1;//神奇!如果输入位置和等于窗口的高 那么输入法就会跑到左上角去
				}
				cpf.ptCurrentPos.x = x;
				cpf.ptCurrentPos.y = y;
				ImmSetCompositionWindow(hIMC, &cpf);
				ImmReleaseContext(_hWnd, hIMC);
			}
			//Debug::Log("%d %d", x, y);
			break;
		}
		case WM_IME_COMPOSITION:
		{
			/*		HIMC        hIMC;
					ImmGetContext(_hWnd);
					hIMC = ImmGetContext(_hWnd);
					char buf[256]{ 0 };
					BOOL b = ImmGetCompositionString(hIMC, GCS_COMPSTR, buf, 255);
					b = ImmReleaseContext(_hWnd, hIMC);*/
			break;
		}
		case WM_ERASEBKGND: {
			break;
		}

		case WM_DISPLAYCHANGE: {
			/*	auto width = LOWORD(lParam);
				auto height = HIWORD(lParam);;
				EzUI::Scale = GetScale();
				SetWindowPos(_hWnd, HWND_TOP, _rect.X * Scale, _rect.Y * Scale, _rect.Width * Scale, _rect.Height * Scale, SWP_NOZORDER | SWP_NOACTIVATE);
				MainLayout->Invalidate();*/
			break;
		}
		case WM_PAINT:
		{
			if (!_load) {
				_load = true;
				OnLoad();
			}
			PAINTSTRUCT pst;
			HDC winHDC = BeginPaint(_hWnd, &pst);
			RECT& r = pst.rcPaint;
			Rect rePaintRect{ r.left,r.top,r.right - r.left, r.bottom - r.top };
			Rending(winHDC, rePaintRect);
			EndPaint(_hWnd, &pst);
			return 0;
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
			if ((wPos->flags & SWP_NOCOPYBITS) == SWP_NOCOPYBITS) {
				rePaint = true;
			}
			//获取客户区的矩形
			RECT rect;
			::GetClientRect(_hWnd, &rect);
			Point clientPoint{ rect.left,rect.top };
			Size clientSize{ rect.right - rect.left,rect.bottom - rect.top };
			//客户区矩形无效的时候
			if (clientSize.Width == 0 && clientSize.Height == 0) {
				//_rect = { 0,0,0,0 };
				return TRUE;
			}
			_rect = Rect{ wPos->x,wPos->y,wPos->cx,wPos->cy };
			_rectClient = Rect{ clientPoint,clientSize };
			//触发
			OnRect(_rect);
			if (!_lastSize.Equals(clientSize)) {
				_lastSize = clientSize;
				OnSize(clientSize);
			}
			if (rePaint) {
				//Debug::Log("SWP_NOCOPYBITS!");
				//丢弃工作区的整个内容。 如果未指定此标志，则会在调整或重新定位窗口后保存并复制回工作区的有效内容。
				Invalidate();

			}
			Point point = _rect.GetLocation();
			if (!_lastPoint.Equals(point)) {
				_lastPoint = point;
				OnMove(point);
			}
			break;
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
		case WM_CHAR: {
			OnKeyChar(wParam, lParam);
			break;
		}
		case WM_KEYDOWN:
		{
			if (wParam == VK_F11) {
				PublicData.Debug = !PublicData.Debug;
				Invalidate();
			}
			OnKeyDown(wParam, lParam);
			break;
		}
		case WM_KEYUP: {
			OnKeyUp(wParam, lParam);
			break;
		}

		case WM_DESTROY:
		{
			OnDestroy();
			break;
		}
		case WM_SETCURSOR: {
			POINT p1{ 0 };
			::GetCursorPos(&p1);
			::ScreenToClient(Hwnd(), &p1);
			Point point{ p1.x,p1.y };
			Point relativePoint;
			Control* outCtl = this->FindControl(point, &relativePoint);//找到当前控件的位置
			HCURSOR cursor = NULL;
			if (outCtl && (cursor = outCtl->GetCursor())) {
				::SetCursor(cursor);
				return TRUE;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme{ 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = _hWnd;
			TrackMouseEvent(&tme);
			OnMouseMove({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
			_mouseIn = true;
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
			Point pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnMouseUp(MouseButton::Left, pt);
			break;
		}
		case WM_RBUTTONUP:
		{
			Point pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnMouseUp(MouseButton::Right, pt);
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
		default:
		{
			break;
		}
		}
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

	void Window::Rending(HDC winHDC, const Rect& rePaintRect) {
#ifdef COUNT_ONPAINT
		StopWatch sw;
#endif // COUNT_ONPAINT
#if USED_Direct2D
		Painter graphics(winHDC, 0, 0, GetClientRect().Width, GetClientRect().Height);
		PaintEventArgs args(graphics);
		args.DC = winHDC;
		args.PublicData = &PublicData;
		args.PublicData->PaintCount = 0;
		args.InvalidRectangle = rePaintRect;
		OnPaint(args);
#endif
#ifdef COUNT_ONPAINT
		char buf[256]{ 0 };
		sprintf(buf, "OnPaint Count(%d) (%d,%d,%d,%d) %dms \n", args.PublicData->PaintCount, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, (int)sw.ElapsedMilliseconds());
		OutputDebugStringA(buf);
#endif // COUNT_ONPAINT
#ifdef DEBUGPAINT
		if (PublicData.Debug) {
			graphics.SetColor(Color::Red);
			graphics.DrawRectangle(rePaintRect);
		}
#endif
	}

	void Window::OnPaint(PaintEventArgs& arg)
	{
		if (MainLayout) {
			MainLayout->Rending(arg);//
		}
	}

	void Window::InitData(const DWORD& ExStyle)
	{
		PublicData.HANDLE = _hWnd;
		PublicData.Window = this;

		if ((ExStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			PublicData.InvalidateRect = [=](void* _rect)->void {
				//Debug::Log("threadId %d",EzUI::GetThreadId());
				RECT r = ((Rect*)_rect)->WinRECT();
				::InvalidateRect(_hWnd, &r, FALSE);
				//::SendMessage(_hWnd, UI_PAINT, (WPARAM)_rect, NULL);
			};
			PublicData.UpdateWindow = [=]()->void {
				::UpdateWindow(_hWnd);
			};
		}

		PublicData.RemoveControl = [=](Control* delControl)->void {
			//Debug::Log("remove delControl %p", delControl);
			if (_focusControl == delControl) {
				_focusControl = NULL;
				//Debug::Log("remove _focusControl %p", delControl);
			}
			if (_inputControl == delControl) {
				_inputControl = NULL;
				//Debug::Log("remove _inputControl %p", delControl);
			}
		};
		PublicData.Notify = [=](Control* sender, EventArgs& args)->bool {
			return OnNotify(sender, args);
		};
		UI_SetUserData(_hWnd, &PublicData);
	}

	bool Window::IsInWindow(Control& pControl, Control& it) {
		Rect& winClientRect = GetClientRect();
		const Rect& rect = it.GetRect();//

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

	Control* Window::FindControl(const Point clientPoint, Point* outPoint) {
		*outPoint = clientPoint;
		Control* outCtl = MainLayout;
	UI_Loop:
		Control* scrollBar = outCtl->GetScrollBar();
		if (scrollBar && scrollBar->GetClientRect().Contains(clientPoint)) {
			if (scrollBar->Visible) {
				auto barRect = scrollBar->GetClientRect();
				(*outPoint).X = clientPoint.X - barRect.X;
				(*outPoint).Y = clientPoint.Y - barRect.Y;
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
				outCtl = &it;
				auto ctlRect = it.GetClientRect();
				(*outPoint).X = clientPoint.X - ctlRect.X;
				(*outPoint).Y = clientPoint.Y - ctlRect.Y;
				goto UI_Loop;
			}
		}
		if (dynamic_cast<Spacer*>(outCtl) && outCtl->Parent) {
			return  outCtl->Parent;
		}
		return outCtl;
	}

	void Window::OnMouseMove(const Point& point)
	{
		if (_inputControl && _mouseDown) { //按住移动的控件
			auto ctlRect = _inputControl->GetClientRect();
			MouseEventArgs args(Event::OnMouseMove, { point.X - ctlRect.X ,point.Y - ctlRect.Y });
			_inputControl->Trigger(args);
			return;
		}

		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);//找到当前控件的位置
		MouseEventArgs args;
		args.Location = relativePoint;

		//触发上一个
		if (_focusControl != outCtl) {
			if (_focusControl) {
				args.EventType = Event::OnMouseLeave;
				_focusControl->Trigger(args);
			}
			_focusControl = outCtl;
		}
		//触发命中的
		if (_focusControl) {
			args.EventType = Event::OnMouseMove;
			_focusControl->Trigger(args);
		}

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
		/*	if (_inputControl) {
				MouseEventArgs args;
				args.Delta = zDelta;
				args.Location = point;
				args.EventType = Event::OnMouseWheel;
				_inputControl->Trigger(args);
			}*/
		ScrollBar* scrollBar = NULL;
		if (_focusControl->GetScrollBar()) {
			scrollBar = dynamic_cast<ScrollBar*>(_focusControl->GetScrollBar());
		}
		Control* pControl = _focusControl;
		while (scrollBar == NULL && pControl)
		{
			if (pControl->GetScrollBar()) {
				scrollBar = dynamic_cast<ScrollBar*>(pControl->GetScrollBar());
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
			//PublicData.UpdateWindow();
		/*	POINT p1{ 0 };
			::GetCursorPos(&p1);
			::ScreenToClient(_hWnd, &p1);
			OnMouseMove({ p1.x, p1.y });*/
		}
	}
	void Window::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
	{
		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);
		if (outCtl) {
			MouseEventArgs args;
			args.Button = mbtn;
			args.Location = relativePoint;
			args.EventType = Event::OnMouseDoubleClick;
			outCtl->Trigger(args);
		}
	}

	void Window::OnMouseDown(MouseButton mbtn, const Point& point)
	{
		::SetCapture(_hWnd);
		//::SetFocus(_hWnd);
		_mouseDown = true;

		//寻早控件
		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);
		MouseEventArgs args;
		args.Button = mbtn;
		args.Location = relativePoint;
		//如果单机的不是上一个 那么上一个触发失去焦点事件
		if (_inputControl != outCtl) {
			if (_inputControl) {
				_inputControl->OnKillFocus();//给上一个输入焦点触发失去焦点的事件
			}
			_inputControl = outCtl;
		}
		//给命中的控件触发鼠标按下事件
		if (_inputControl) {
			args.EventType = Event::OnMouseDown;
			_inputControl->Trigger(args);
		}
	}
	void Window::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		::ReleaseCapture();
		if (_mouseDown == false) {
			return;
		}
		_mouseDown = false;

		if (_inputControl) {
			auto ctlRect = _inputControl->GetClientRect();
			MouseEventArgs args;
			args.Button = mbtn;
			args.Location = { point.X - ctlRect.X,point.Y - ctlRect.Y };
			args.EventType = Event::OnMouseUp;
			_inputControl->Trigger(args);//触发鼠标抬起事件

			if (_inputControl && ctlRect.Contains(point)) {//如果焦点还在并且鼠标未移出控件内 触发click事件
				args.EventType = Event::OnMouseClick;
				_inputControl->Trigger(args);
			}
			if (_inputControl && !ctlRect.Contains(point))//如果焦点还在 但是鼠标已经不在控件矩形内 触发鼠标移出事件
			{
				args.EventType = Event::OnMouseLeave;
				_inputControl->Trigger(args);
			}
		}

		//做双击消息处理
		auto _time = std::chrono::system_clock::now();
		auto diff = _time - _lastDownTime;
		auto timeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		if (timeOffset < 300 && _lastBtn == mbtn) {//300毫秒之内同一个按钮按下两次算双击消息
			_lastDownTime = std::chrono::system_clock::from_time_t(0);
			OnMouseDoubleClick(mbtn, point);
		}
		_lastBtn = mbtn;
		_lastDownTime = _time;
		//OnMouseMove(point);
	}

	void Window::OnMouseClick(MouseButton mbtn, const Point& point) {

	}

	void Window::OnSize(const Size& sz)
	{
		if (!MainLayout) {
			return;
		}
#ifdef COUNT_ONSIZE
		StopWatch sw;
#endif
		__count_onsize = 0;
		MainLayout->SetRect(this->GetClientRect());
		MainLayout->Invalidate();
#ifdef COUNT_ONSIZE
		Debug::Log("OnSize Count(%d) (%d,%d) %dms\n", __count_onsize, sz.Width, sz.Height, sw.ElapsedMilliseconds());
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

	void Window::OnKeyChar(WPARAM wParam, LPARAM lParam)
	{
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyChar, wParam, lParam);
			_inputControl->Trigger(args);
			return;
		}
	}
	void Window::OnKeyDown(WPARAM wParam, LPARAM lParam) {
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyDown, wParam, lParam);
			_inputControl->Trigger(args);
			return;
		}
	}
	void Window::OnKeyUp(WPARAM wParam, LPARAM lParam) {
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyUp, wParam, lParam);
			_inputControl->Trigger(args);
			return;
		}
	}
	void Window::OnMove(const Point& point) {

	}
	LRESULT Window::ZoomWindow(const  LPARAM& lParam) {
		RECT rc;
		GetWindowRect(Hwnd(), &rc);
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
	void Window::MoveWindow() {
		::ReleaseCapture();
		SendMessage(Hwnd(), WM_NCLBUTTONDOWN, HTCAPTION, NULL);//模拟鼠标按住标题栏移动窗口
		SendMessage(Hwnd(), WM_LBUTTONUP, NULL, NULL);//松开
	}
	bool Window::OnNotify(Control* sender, EventArgs& args) {
		if (args.EventType == Event::OnMouseDown) {
			if (sender->Action == ControlAction::MoveWindow || sender == MainLayout) {
				MoveWindow();
				return true;
			}
			if (sender->Action == ControlAction::Mini) {
				Show(SW_MINIMIZE);
				return false;
			}
			if (sender->Action == ControlAction::Max) {
				if (!IsZoomed(_hWnd)) {
					this->ShowMax();
				}
				else {
					Show(SW_SHOWNORMAL);
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
					//tabLayout->ResumeLayout();
					tabLayout->Invalidate();
				}
			}
		}
		return false;
	}

};