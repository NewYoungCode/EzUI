#include "Window.h"
#include "TabLayout.h"

namespace EzUI {
#define _focusControl PublicData.FocusControl
#define _inputControl PublicData.InputControl

	Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
	{
		MonitorInfo monitorInfo;
		EzUI::GetMontior(&monitorInfo);
		int x = monitorInfo.WorkRect.X;
		int y = monitorInfo.WorkRect.Y;
		int sw = monitorInfo.WorkRect.Width;//当前工作区域的宽
		int sh = monitorInfo.WorkRect.Height;//当前工作区域的高

		_rect.X = x + (sw - width) / 2;//保证左右居中
		_rect.Y = y + (sh - height) / 2;//保证上下居中
		_rect.Width = width;
		_rect.Height = height;

		_hWnd = ::CreateWindowExW(ExStyle | WS_EX_ACCEPTFILES, WindowClassName, WindowClassName, dStyle,
			_rect.X, _rect.Y, width, height, owner, NULL, GetModuleHandle(NULL), NULL);
		InitData(ExStyle);//设置基本数据
	}

	Window::~Window()
	{
		if (__rollTimer) {
			delete __rollTimer;
		}
		if (::IsWindow(Hwnd())) {
			::DestroyWindow(Hwnd());
		}
	}

	Control* Window::FindControl(const EString& objectName)
	{
		if (!MainLayout) {
			return NULL;
		}
		return this->MainLayout->FindControl(objectName);
	}


	const HWND& Window::Hwnd()
	{
		return _hWnd;
	}
	const Rect& Window::GetWindowRect()
	{
		if (_rect.IsEmptyArea()) {
			RECT rect;
			::GetWindowRect(_hWnd, &rect);
			_rect = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
		}
		return _rect;
	}
	const Rect& Window::GetClientRect()
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
	void Window::SetTopMost(bool top)
	{
		if (top) {
			::SetWindowPos(Hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else {
			::SetWindowPos(Hwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	bool Window::IsTopMost()
	{
		LONG_PTR exStyle = ::GetWindowLongPtr(Hwnd(), GWL_EXSTYLE);
		bool isTop = ((exStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST);
		return isTop;
	}
	void Window::SetSize(const Size& size) {
		const Rect& rect = GetWindowRect();
		::MoveWindow(Hwnd(), rect.X, rect.Y, size.Width, size.Height, FALSE);
	}
	void Window::SetLocation(const Point& pt) {
		const Rect& rect = GetWindowRect();
		::MoveWindow(Hwnd(), pt.X, pt.Y, rect.Width, rect.Height, FALSE);
	}
	void Window::SetRect(const Rect& rect)
	{
		::MoveWindow(Hwnd(), rect.X, rect.Y, rect.Width, rect.Height, FALSE);
	}
	void Window::SetMiniSize(const Size& size)
	{
		_miniSize = size;
	}
	void Window::SetMaxSize(const Size& size)
	{
		_maxSize = size;
	}
	void Window::SetIcon(short id)
	{
		SetIcon(::LoadIcon(::GetModuleHandle(0), MAKEINTRESOURCE(id)));//
	}
	void Window::SetIcon(HICON icon)
	{
		::SendMessage(Hwnd(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
	}
	void Window::SetLayout(EzUI::Control* layout) {
		ASSERT(layout);
		MainLayout = layout;
		if (MainLayout->Style.FontFamily.empty()) {
			MainLayout->Style.FontFamily = L"Microsoft YaHei";
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
		::SendMessage(Hwnd(), WM_CLOSE, 0, 0);
	}
	void Window::Show(int cmdShow)
	{
		ASSERT(MainLayout);
		::ShowWindow(Hwnd(), cmdShow);
		if (IsVisible()) {
			::UpdateWindow(Hwnd());
		}
	}
	void Window::ShowNormal()
	{
		Show(SW_SHOWNORMAL);
	}
	void Window::ShowMinimized() {
		Show(SW_MINIMIZE);
	}
	void Window::ShowMaximized() {
		Show(SW_MAXIMIZE);
	}
	int Window::ShowModal(bool wait)
	{
		_OwnerHwnd = ::GetWindowOwner(Hwnd());
		Show();
		if (_OwnerHwnd) {
			::EnableWindow(_OwnerHwnd, FALSE);
		}
		if (wait) {//
			MSG msg{ 0 };
			while (::IsWindow(Hwnd()) && ::GetMessage(&msg, NULL, 0, 0) && msg.message != WM_QUIT)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) {//
				::PostQuitMessage(msg.wParam);
				return _closeCode;
			}
		}
		if (_OwnerHwnd) {
			::SetForegroundWindow(_OwnerHwnd);
		}
		return _closeCode;
	}
	void Window::Hide() {
		Show(SW_HIDE);
	}
	bool Window::IsVisible() {
		return ::IsWindowVisible(Hwnd()) ? true : false;
	}
	void Window::SetVisible(bool flag) {
		if (flag) {
			Show(SW_RESTORE);
			::SetForegroundWindow(Hwnd());
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
		case WM_MOVE: {
			int xPos = (int)(short)LOWORD(lParam);   // horizontal position 
			int yPos = (int)(short)HIWORD(lParam);   // vertical position 
			int p = 0;
			break;
		}
		case WM_SIZE: {
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			int p = 0;
			break;
		}
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pMMInfo = (MINMAXINFO*)lParam;
			if (!_miniSize.Empty()) {
				pMMInfo->ptMinTrackSize.x = _miniSize.Width;
				pMMInfo->ptMinTrackSize.y = _miniSize.Height;
			}
			if (!_maxSize.Empty()) {
				pMMInfo->ptMaxTrackSize.x = _maxSize.Width;
				pMMInfo->ptMaxTrackSize.y = _maxSize.Height;
				break;
			}
			MONITORINFO monitor;
			monitor.cbSize = sizeof(monitor);
			::GetMonitorInfo(::MonitorFromWindow(Hwnd(), MONITOR_DEFAULTTOPRIMARY), &monitor);
			//是否为主显示器
			if ((monitor.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY) {
				//保证窗口在最大化的时候始终在工作区 不会遮挡任务栏
				RECT& rcWork = monitor.rcWork;
				pMMInfo->ptMaxPosition.x = rcWork.left;
				pMMInfo->ptMaxPosition.y = rcWork.top;
				pMMInfo->ptMaxSize.x = rcWork.right - rcWork.left;
				pMMInfo->ptMaxSize.y = rcWork.bottom - rcWork.top;
			}
			else {
				//获取主显示器
				HMONITOR hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
				MONITORINFO monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				::GetMonitorInfo(hMonitor, &monitorInfo);
				RECT& rcWork = monitorInfo.rcMonitor;
				pMMInfo->ptMaxPosition.x = rcWork.left;
				pMMInfo->ptMaxPosition.y = rcWork.top;
				pMMInfo->ptMaxSize.x = rcWork.right - rcWork.left;
				pMMInfo->ptMaxSize.y = rcWork.bottom - rcWork.top;
			}
			//Debug::Log("%d %d %d %d", pMMInfo->ptMaxPosition.x, pMMInfo->ptMaxPosition.y, pMMInfo->ptMaxSize.x, pMMInfo->ptMaxSize.y);
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
			auto width = LOWORD(lParam);
			auto height = HIWORD(lParam);;
			//	EzUI::Scale = GetScale();
				//SetWindowPos(_hWnd, HWND_TOP, _rect.X * Scale, _rect.Y * Scale, _rect.Width * Scale, _rect.Height * Scale, SWP_NOZORDER | SWP_NOACTIVATE);
				//MainLayout->Invalidate();
			break;
		}
		case WM_DPICHANGED:
		{
			int dpi = HIWORD(wParam);
			//新的缩放比
			FLOAT newScale = (float)dpi / USER_DEFAULT_SCREEN_DPI;
			RECT* const prcNewWindow = (RECT*)lParam;
			const Rect& oldRect = this->GetWindowRect();
			int newX = prcNewWindow->left;
			int newY = prcNewWindow->top;
			int newWidth = prcNewWindow->right - prcNewWindow->left;
			int newHeight = prcNewWindow->bottom - prcNewWindow->top;
			//不同屏幕拖拽时缩放 (不同屏幕拖拽会先触发WM_DPICHANGED)可以使用上次的宽度进行计算比例
			int lastWidth = this->_lastSize.Width;
			if (newWidth != lastWidth) {
				EzUI::Scale = newScale = (float)newWidth / lastWidth;
			}
			else {
				//同屏缩放会先触发WM_SIZE所以这里不能这么做 逻辑还不对!!!
				EzUI::Scale = newScale / EzUI::Scale;
			}
			DpiChangeEventArgs arg(EzUI::Scale);
			MainLayout->DispatchEvent(arg);
			SetWindowPos(Hwnd(), NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
			this->OnSize({ newWidth,newHeight });
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT pst;
			HDC winHDC = ::BeginPaint(Hwnd(), &pst);
			RECT& r = pst.rcPaint;
			Rect rePaintRect{ r.left,r.top,r.right - r.left, r.bottom - r.top };
			this->DoPaint(winHDC, rePaintRect);
			::EndPaint(Hwnd(), &pst);
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
			::GetClientRect(Hwnd(), &rect);
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
				OnLocation(point);
			}
			break;
		}
		case WM_CLOSE:
		{
			bool bClose = true;
			OnClose(bClose);
			if (bClose) {
				//开始关闭窗口
				if (_OwnerHwnd) {
					::EnableWindow(_OwnerHwnd, TRUE);
					_OwnerHwnd = NULL;
				}
				::DestroyWindow(_hWnd);
			}
			else {
				//关闭已取消
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
			if (::IsWindowEnabled(Hwnd())) {
				POINT p1{ 0 };
				::GetCursorPos(&p1);
				::ScreenToClient(Hwnd(), &p1);
				Point point{ p1.x,p1.y };
				Point relativePoint;
				Control* outCtl = this->FindControl(point, &relativePoint);//找到当前控件的位置
				if (outCtl && outCtl->Enable) {
					if (outCtl->HoverStyle.Cursor != NULL) {
						::SetCursor(outCtl->HoverStyle.Cursor);
						return TRUE;
					}
					else if (outCtl->Style.Cursor != NULL) {
						::SetCursor(outCtl->Style.Cursor);
						return TRUE;
					}
				}
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme{ 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = Hwnd();
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
			//OnMouseWheel(5, zDelta, { xPos,yPos });
			OnMouseWheelSlower(zDelta, { xPos,yPos });
			break;
		}
		default:
		{
			break;
		}
		}
		return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
	}

	void Window::DoPaint(HDC winHDC, const Rect& rePaintRect) {
#define COUNT_ONPAINT 0
#if COUNT_ONPAINT
		StopWatch sw;
#endif // COUNT_ONPAINT
#if USED_DIRECT2D
		DXRender graphics(winHDC, 0, 0, GetClientRect().Width, GetClientRect().Height);
		PaintEventArgs args(graphics);
		args.DC = winHDC;
		args.PublicData = &PublicData;
		args.PublicData->PaintCount = 0;
		args.InvalidRectangle = rePaintRect;
		OnPaint(args);
#endif
#if COUNT_ONPAINT
		char buf[256]{ 0 };
		sprintf(buf, "OnPaint Count(%d) (%d,%d,%d,%d) %dms \n", args.PublicData->PaintCount, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, (int)sw.ElapsedMilliseconds());
		OutputDebugStringA(buf);
#endif // COUNT_ONPAINT

	}

	void Window::OnPaint(PaintEventArgs& arg)
	{
		if (MainLayout) {
			MainLayout->DispatchEvent(arg);//
		}
	}

	void Window::InitData(const DWORD& ExStyle)
	{
		PublicData.HANDLE = Hwnd();
		PublicData.Window = this;

		if ((ExStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			PublicData.InvalidateRect = [=](void* _rect)->void {
				RECT r;
				r.left = ((Rect*)_rect)->GetLeft();
				r.top = ((Rect*)_rect)->GetTop();
				r.right = ((Rect*)_rect)->GetRight();
				r.bottom = ((Rect*)_rect)->GetBottom();
				::InvalidateRect(Hwnd(), &r, TRUE);
			};
			PublicData.UpdateWindow = [=]()->void {
				::UpdateWindow(Hwnd());
			};
		}

		PublicData.RemoveControl = [=](Control* delControl)->void {
			if (_focusControl == delControl) {
				_focusControl = NULL;
			}
			if (_inputControl == delControl) {
				_inputControl = NULL;
			}
		};
		PublicData.Notify = [=](Control* sender, EventArgs& args)->bool {
			return OnNotify(sender, args);
		};
		UI_SET_USERDATA(_hWnd, &PublicData);
	}

	bool Window::IsInWindow(Control& pControl, Control& it) {
		const Rect& winClientRect = GetClientRect();
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
	Find_Loop:
		ScrollBar* scrollBar = outCtl->GetScrollBar();
		if (scrollBar && scrollBar->GetClientRect().Contains(clientPoint)) {
			if (scrollBar->IsDraw()) {
				auto barRect = scrollBar->GetClientRect();
				(*outPoint).X = clientPoint.X - barRect.X;
				(*outPoint).Y = clientPoint.Y - barRect.Y;
				outCtl = scrollBar;
				return outCtl;
			}
		}

		std::list<Control*>* pTemp;
		if (outCtl->VisibleControls.size() > 0) {
			pTemp = &outCtl->VisibleControls;
		}
		else {
			pTemp = (std::list<Control*>*)(&(outCtl->GetControls()));
		}

		for (auto i = pTemp->rbegin(); i != pTemp->rend(); i++) {
			Control& it = **i;
			if (!it.IsVisible()) {
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
				goto Find_Loop;
			}
		}
		/*Spacer* isSpacer = dynamic_cast<Spacer*>(outCtl);
		if ((isSpacer || outCtl->MousePassThrough != 0) && outCtl->Parent) {
			return  outCtl->Parent;
		}*/
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
			_inputControl->DispatchEvent(args);
			return;
		}

		Point relativePoint;
		Control* newCtl = this->FindControl(point, &relativePoint);//找到当前控件的位置
		MouseEventArgs args(Event::None);
		args.Location = relativePoint;

		bool ok = true;
		//触发上一个
		if (_focusControl != newCtl) {
			if (_focusControl) {
				args.EventType = Event::OnMouseLeave;
				ok = _focusControl->DispatchEvent(args);
			}
			ok = newCtl->DispatchEvent(Event::OnMouseEnter);
			if (ok) {
				_focusControl = newCtl;
			}
		}
		//触发命中的
		if (_focusControl) {
			args.EventType = Event::OnMouseMove;
			_focusControl->DispatchEvent(args);
		}

	}
	void Window::OnMouseLeave()
	{
		if (_focusControl) {
			MouseEventArgs args(Event::OnMouseLeave);
			_focusControl->DispatchEvent(args);
		}
		_focusControl = NULL;
		_mouseDown = false;
	}

	void Window::OnMouseWheelSlower(short zDelta, const Point& point) {
		if (__rollTimer == NULL) {
			__rollTimer = new Windows::Timer;
			__rollTimer->Interval = 5;
		}
		__rollTimer->Tick = [this, zDelta, point](Windows::Timer* tm)->void {
			_rollCount--;
			this->OnMouseWheel(_rollCount, zDelta, point);
			if (_rollCount <= 0) {
				tm->Stop();
			}
		};
		int fx = (zDelta > 0 ? -5 : 5);//滚动方向
		if (fx != _rollSpeed) {//如果滚动方向与上次不同 即可停止
			_rollCount = 1;//滚动一次
		}
		else {
			_rollCount += 3;//将滚动五次
		}
		_rollSpeed = fx;//控制方向与速度
		__rollTimer->Start();
	}
	void Window::OnMouseWheel(int rollCount, short zDelta, const Point& point)
	{
		if (_focusControl == NULL) return;
		if (_focusControl) {
			MouseEventArgs args(Event::OnMouseWheel);
			args.ZDelta = zDelta;
			args.Location = point;
			args.RollCount = rollCount;
			_focusControl->DispatchEvent(args);
		}
		ScrollBar* scrollBar = NULL;
		if (_focusControl && _focusControl->GetScrollBar() && _focusControl->GetScrollBar()->Scrollable()) {
			scrollBar = dynamic_cast<ScrollBar*>(_focusControl->GetScrollBar());
		}
		Control* pControl = _focusControl;
		while (scrollBar == NULL && pControl)
		{
			if (pControl->GetScrollBar() && pControl->GetScrollBar()->Scrollable()) {
				scrollBar = dynamic_cast<ScrollBar*>(pControl->GetScrollBar());
				break;
			}
			pControl = pControl->Parent;
		}
		if (scrollBar) {
			MouseEventArgs args(Event::OnMouseWheel);
			args.ZDelta = zDelta;
			args.Location = point;
			args.RollCount = rollCount;
			scrollBar->DispatchEvent(args);

			//取消注释即可实时跟踪控件刷新状态 但是感觉没什么必要
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
			MouseEventArgs args(Event::OnMouseDoubleClick);
			args.Button = mbtn;
			args.Location = relativePoint;
			outCtl->DispatchEvent(args);
		}
	}

	void Window::OnMouseDown(MouseButton mbtn, const Point& point)
	{
		::SetCapture(_hWnd);
		_mouseDown = true;
		//寻早控件
		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);
		//如果单机的不是上一个 那么上一个触发失去焦点事件
		if (_inputControl != outCtl) {
			if (_inputControl) {
				KillFocusEventArgs arg(outCtl);
				_inputControl->DispatchEvent(arg);//给上一个输入焦点触发失去焦点的事件
			}
			_inputControl = outCtl;
		}
		//给命中的控件触发鼠标按下事件
		if (_inputControl) {
			MouseEventArgs args(Event::OnMouseDown);
			args.Button = mbtn;
			args.Location = relativePoint;
			_inputControl->DispatchEvent(args);
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
	}
	void Window::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		::ReleaseCapture();
		if (_mouseDown == false) {
			return;
		}
		_mouseDown = false;

		if (_inputControl) {
			Rect ctlRect = _inputControl->GetClientRect();
			MouseEventArgs args(Event::None);
			args.Button = mbtn;
			args.Location = { point.X - ctlRect.X,point.Y - ctlRect.Y };
			if (_inputControl && mbtn == _lastBtn) {//如果焦点还在并且鼠标未移出控件内 触发click事件
				args.EventType = Event::OnMouseClick;
				_inputControl->DispatchEvent(args);
			}
			if (_inputControl) {
				POINT p1{ 0 };
				::GetCursorPos(&p1);
				::ScreenToClient(Hwnd(), &p1);
				args.EventType = Event::OnMouseUp;
				ctlRect = _inputControl->GetClientRect();
				bool inRect = false;
				if (ctlRect.Contains(p1.x, p1.y) && ::GetForegroundWindow() == Hwnd()) {
					_inputControl->State = ControlState::Hover;
					inRect = true;
				}
				_inputControl->DispatchEvent(args);//触发鼠标抬起事件
				if (!inRect && _inputControl)
				{
					args.EventType = Event::OnMouseLeave;
					_inputControl->DispatchEvent(args);//触发离开事件
				}
			}
		}
	}

	void Window::OnMouseClick(MouseButton mbtn, const Point& point) {

	}

	void Window::OnSize(const Size& sz)
	{
		if (!MainLayout) {
			return;
		}

		MainLayout->SetRect(this->GetClientRect());
		//MainLayout->Invalidate();
	}

	void Window::OnRect(const Rect& rect)
	{
	}
	void Window::OnClose(bool& bClose)
	{
	}
	void Window::OnDestroy()
	{
	}
	void Window::OnKeyChar(WPARAM wParam, LPARAM lParam)
	{
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyChar, wParam, lParam);
			_inputControl->DispatchEvent(args);
			return;
		}
	}
	void Window::OnKeyDown(WPARAM wParam, LPARAM lParam) {
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyDown, wParam, lParam);
			_inputControl->DispatchEvent(args);
			return;
		}
	}
	void Window::OnKeyUp(WPARAM wParam, LPARAM lParam) {
		if (_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyUp, wParam, lParam);
			_inputControl->DispatchEvent(args);
			return;
		}
	}
	void Window::OnLocation(const Point& point) {
	}
	void Window::OnKillFocus(HWND hWnd)
	{
	}
	LRESULT Window::ZoomWindow(const  LPARAM& lParam) {
		RECT rc;
		::GetWindowRect(Hwnd(), &rc);
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
		return HTCLIENT;//
	}
	void Window::MoveWindow() {
		::ReleaseCapture();
		SendMessage(Hwnd(), WM_NCLBUTTONDOWN, HTCAPTION, NULL);//模拟鼠标按住标题栏移动窗口
		SendMessage(Hwnd(), WM_LBUTTONUP, NULL, NULL);//松开
	}
	bool Window::OnNotify(Control* sender, EventArgs& args) {
		if (args.EventType == Event::OnMouseDoubleClick) {
			if (sender->Action == ControlAction::MoveWindow || sender == MainLayout) {
				if (this->Zoom) {
					if (::IsZoomed(Hwnd())) {
						this->ShowNormal();
					}
					else {
						this->ShowMaximized();
					}
				}
			}
			return false;
		}
		if (args.EventType == Event::OnMouseDown) {
			if (sender->Action == ControlAction::MoveWindow || sender == MainLayout) {
				MoveWindow();
				return false;
			}
			if (sender->Action == ControlAction::Mini) {
				this->ShowMinimized();
				return false;
			}
			if (sender->Action == ControlAction::Max) {
				if (!IsZoomed(_hWnd)) {
					this->ShowMaximized();
				}
				else {
					this->ShowNormal();
				}
				return false;
			}
			if (sender->Action == ControlAction::Close) {
				MouseEventArgs args(Event::OnMouseLeave);
				sender->DispatchEvent(args);
				this->Close();
				return false;
			}
		}
		if (args.EventType == Event::OnMouseClick) {
			//鼠标左侧按钮单击
			//if (args.EventType == Event::OnMouseClick && ((MouseEventArgs&)(args)).Button == MouseButton::Left) {
			EString  ctlName = sender->GetAttribute("tablayout");
			if (!ctlName.empty()) {
				auto ctls = sender->Parent->FindControl("tablayout", ctlName);
				TabLayout* tabLayout = dynamic_cast<TabLayout*>(FindControl(ctlName));
				if (tabLayout && sender->Parent) {
					size_t pos = 0;
					for (auto& it : ctls)
					{
						if (it == sender) {
							tabLayout->SetPageIndex(pos);
							//tabLayout->ResumeLayout();
							tabLayout->Invalidate();
							break;
						}
						pos++;
					}
				}
			}

		}
		return false;
	}

};