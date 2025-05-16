#include "Window.h"
#include "TabLayout.h"

namespace EzUI {
#define _focusControl PublicData->FocusControl
#define _inputControl PublicData->InputControl

	Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
	{
		InitWindow(width, height, owner, dStyle, ExStyle);//设置基本数据
	}

	Window::~Window()
	{
		if (_hWndTips) {
			::DestroyWindow(_hWndTips);
		}
		if (Hwnd()) {
			::DestroyWindow(Hwnd());
		}
		if (PublicData) {
			delete PublicData;
		}
	}

	void Window::InitWindow(int width, int height, HWND owner, DWORD dStyle, DWORD  exStyle)
	{
		this->PublicData = new WindowData;
		(bool&)this->IsWindow = true;
		Rect _rect(0, 0, width, height);

		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		for (auto& it : EzUI::__EzUI__MonitorInfos) {
			Rect rect = it.Rect;
			rect.Width = it.Physical.Width;
			rect.Height = it.Physical.Height;
			if (rect.Contains(cursorPos.x, cursorPos.y)) {
				_rect.X = rect.X;
				_rect.Y = rect.Y;
				this->PublicData->Scale = it.Scale;
				break;
			}
		}

		_rect.Scale(this->PublicData->Scale);

		PublicData->HANDLE = ::CreateWindowExW(exStyle | WS_EX_ACCEPTFILES, EzUI::__EzUI__WindowClassName, EzUI::__EzUI__WindowClassName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dStyle,
			_rect.X, _rect.Y, _rect.Width, _rect.Height, owner, NULL, EzUI::__EzUI__HINSTANCE, NULL);
		PublicData->Window = this;
		if (owner) {
			this->CenterToWindow(owner);
		}
		else {
			this->CenterToScreen();
		}

		//创建冒泡提示窗口
		_hWndTips = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS,
			NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			Hwnd(),
			NULL,
			EzUI::__EzUI__HINSTANCE,
			NULL
		);

		PublicData->HANDLE = Hwnd();
		PublicData->Window = this;

		if ((exStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			PublicData->InvalidateRect = [this](const Rect& rect)->void {
				RECT r;
				r.left = rect.GetLeft();
				r.top = rect.GetTop();
				r.right = rect.GetRight();
				r.bottom = rect.GetBottom();
				::InvalidateRect(Hwnd(), &r, FALSE);
				};
			PublicData->UpdateWindow = [this]()->void {
				RECT updateRect;
				while (::GetUpdateRect(Hwnd(), &updateRect, FALSE))
				{
					Rect rect(updateRect);
					if (updateRect.left == 0 && updateRect.top == 0 && updateRect.left == 0 && updateRect.right == 0) {
						break;
					}
					::UpdateWindow(Hwnd());
				}
				};
		}
		PublicData->SetTips = [this](Control* ctl, const std::wstring& text)->void {

			// 枚举并删除每个提示项
			int toolCount = SendMessage(_hWndTips, TTM_GETTOOLCOUNT, 0, 0);
			for (int i = 0; i < toolCount; ++i) {
				TOOLINFO toolInfo{ 0 };
				toolInfo.cbSize = sizeof(TOOLINFO);
				toolInfo.hwnd = Hwnd();
				toolInfo.uFlags = TTF_IDISHWND;
				toolInfo.hwnd = _hWndTips;
				// 发送 TTM_ENUMTOOLS 消息以获取提示项信息
				if (SendMessage(_hWndTips, TTM_ENUMTOOLS, i, (LPARAM)&toolInfo)) {
					// 发送 TTM_DELTOOL 消息删除提示项
					SendMessage(_hWndTips, TTM_DELTOOL, 0, (LPARAM)&toolInfo);
				}
			}
			//EString str;
			//EString::UnicodeToANSI(text, &str);
			//printf("tips: %s\n", str.c_str());
			if (!text.empty()) {
				// 发送 TTM_DELALLTOOL 消息
				TOOLINFO	tti{ 0 };
				tti.cbSize = sizeof(TOOLINFO);
				tti.uFlags = TTF_SUBCLASS;// | TTF_TRACK;
				tti.hwnd = Hwnd();
				tti.rect = ctl->GetViewRect().ToRECT();
				tti.uId = (UINT_PTR)ctl;
				tti.lpszText = (LPWSTR)text.c_str();
				//添加一个tips信息
				SendMessage(_hWndTips, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
			}
			};
		PublicData->DelTips = [this](Control* ctl)->void {
			TOOLINFO	tti{ 0 };
			tti.cbSize = sizeof(TOOLINFO);
			tti.hwnd = Hwnd();
			tti.uId = (UINT_PTR)ctl;
			//移除
			SendMessage(_hWndTips, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
			};
		PublicData->RemoveControl = [this](Control* delControl)->void {
			if (_focusControl == delControl) {
				_focusControl = NULL;
			}
			if (_inputControl == delControl) {
				_inputControl = NULL;
			}
			};
		PublicData->Notify = [this](Control* sender, EventArgs& args)->bool {
			return OnNotify(sender, args);
			};
		UI_SET_USERDATA(Hwnd(), this->PublicData);
	}

	Control* Window::FindControl(const EString& objectName)
	{
		if (!_layout) {
			return NULL;
		}
		return this->_layout->FindControl(objectName);
	}

	HWND Window::Hwnd()
	{
		return PublicData->HANDLE;
	}
	const Rect& Window::GetWindowRect()
	{
		if (_rect.IsEmptyArea()) {
			RECT rect;
			::GetWindowRect(Hwnd(), &rect);
			_rect = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
		}
		return _rect;
	}
	const Rect& Window::GetClientRect()
	{
		if (_rectClient.IsEmptyArea()) {
			RECT rect;
			::GetClientRect(Hwnd(), &rect);
			_rectClient = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
		}
		return _rectClient;
	}

	void Window::SetText(const EString& text)
	{
		::SetWindowTextW(Hwnd(), text.unicode().c_str());
	}

	EString Window::GetText() {
		WCHAR buf[513]{ 0 };
		::GetWindowTextW(Hwnd(), buf, 512);
		return buf;
	}

	void Window::SetTopMost(bool top)
	{
		::SetWindowPos(Hwnd(), top ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	bool Window::IsTopMost()
	{
		LONG_PTR exStyle = ::GetWindowLongPtr(Hwnd(), GWL_EXSTYLE);
		bool isTop = ((exStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST);
		return isTop;
	}
	void Window::SetSize(const Size& size) {
		const Rect& rect = GetWindowRect();
		this->SetRect(Rect(rect.X, rect.Y, size.Width, size.Height));
	}
	void Window::SetLocation(const Point& pt) {
		const Rect& rect = GetWindowRect();
		this->SetRect(Rect(pt.X, pt.Y, rect.Width, rect.Height));
	}
	void Window::SetRect(const Rect& rect)
	{
		::SetWindowPos(Hwnd(), NULL, rect.X, rect.Y, rect.Width, rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	void Window::SetMiniSize(const Size& size)
	{
		_miniSize = size;
		_miniSize.Scale(this->PublicData->Scale);
	}
	void Window::SetMaxSize(const Size& size)
	{
		_maxSize = size;
		_maxSize.Scale(this->PublicData->Scale);
	}
	void Window::SetIcon(short id)
	{
		SetIcon(::LoadIcon(::EzUI::__EzUI__HINSTANCE, MAKEINTRESOURCE(id)));//
	}
	void Window::SetIcon(HICON icon)
	{
		::SendMessage(Hwnd(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
	}
	void Window::SetLayout(EzUI::Control* layout) {
		ASSERT(layout);
		_layout = layout;
		_layout->PublicData = this->PublicData;

		if (_layout->Style.FontFamily.empty()) {
			WCHAR fontName[LF_FACESIZE] = { 0 };
			NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
			// 获取系统非客户区字体
			if (::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
			{
				::lstrcpyW(fontName, ncm.lfMessageFont.lfFaceName);
			}
			else
			{
				// 获取系统普通文本字体
				::SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &fontName, 0);
			}
			_layout->Style.FontFamily = fontName;
		}
		if (_layout->Style.FontSize == 0) {
			_layout->Style.FontSize = 12;
		}
		if (_layout->Style.ForeColor.GetValue() == 0) {
			_layout->Style.ForeColor = Color::Black;
		}
		if (_layout->Style.BackColor.GetValue() == 0) {
			LONG_PTR exStyle = ::GetWindowLongPtr(Hwnd(), GWL_EXSTYLE);
			bool isLayered = ((exStyle & WS_EX_LAYERED) != 0);
			if (!isLayered) {//非layered窗口 主布局控件必须设置背景色
				_layout->Style.BackColor = Color::White;
			}
		}
		if (_layout->GetScale() != this->GetScale()) {
			_layout->DispatchEvent(DpiChangeEventArgs(this->GetScale()));
		}
		_layout->SetRect(this->GetClientRect());
	}

	Control* Window::GetLayout()
	{
		return this->_layout;
	}
	void Window::Close(int code) {
		_closeCode = code;
		::SendMessage(Hwnd(), WM_CLOSE, 0, 0);
	}
	void Window::Show()
	{
		ASSERT(_layout);
		::ShowWindow(Hwnd(), SW_SHOW);
		_layout->Refresh();
	}
	void Window::Show(int cmdShow)
	{
		::ShowWindow(Hwnd(), cmdShow);
		if (IsVisible()) {
			ASSERT(_layout);
			_layout->Refresh();
		}
	}
	void Window::ShowNormal()
	{
		::ShowWindow(Hwnd(), SW_SHOWNORMAL);
	}
	void Window::ShowMinimized() {
		::ShowWindow(Hwnd(), SW_MINIMIZE);
	}
	void Window::ShowMaximized() {
		::ShowWindow(Hwnd(), SW_MAXIMIZE);
	}
	void Window::ShowFullScreen()
	{
		::ShowWindow(Hwnd(), SW_MAX);
		MonitorInfo monitorInfo;
		EzUI::GetMontior(&monitorInfo, Hwnd());
		::SetWindowPos(Hwnd(), HWND_TOP, monitorInfo.Rect.X, monitorInfo.Rect.Y, monitorInfo.Rect.Width, monitorInfo.Rect.Height, NULL);
		::ShowWindow(Hwnd(), SW_SHOW);
	}
	int Window::ShowModal(bool disableOnwer)
	{
		//此处代码不能随意更改 解决关闭窗口时,owner窗口闪烁问题
		if (disableOnwer) {
			_oWnerWnd = ::GetWindowOwner(Hwnd());
		}
		if (_oWnerWnd) {
			::EnableWindow(_oWnerWnd, FALSE);
		}
		Show();
		::MSG msg{ 0 };
		while (Hwnd() && ::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (_oWnerWnd) {
			::SetFocus(_oWnerWnd);
			_oWnerWnd = NULL;
		}
		return _closeCode;
	}
	void Window::Hide() {
		::ShowWindow(Hwnd(), SW_HIDE);
	}
	bool Window::IsVisible() {
		return ::IsWindowVisible(Hwnd()) ? true : false;
	}
	void Window::SetVisible(bool flag) {
		if (flag) {
			::ShowWindow(Hwnd(), SW_RESTORE);
			::SetForegroundWindow(Hwnd());
		}
		else {
			Hide();
		}
	}

	void Window::Invalidate()
	{
		if (_layout) {
			_layout->Invalidate();
		}
	}
	void Window::Refresh()
	{
		if (_layout) {
			_layout->Refresh();
		}
	}

	void Window::CenterToScreen()
	{
		MonitorInfo monitorInfo;
		EzUI::GetMontior(&monitorInfo);

		int x = monitorInfo.WorkRect.X;
		int y = monitorInfo.WorkRect.Y;
		int sw = monitorInfo.WorkRect.Width;//当前工作区域的宽
		int sh = monitorInfo.WorkRect.Height;//当前工作区域的高

		Rect _rect = this->GetWindowRect();
		int width = _rect.Width;
		int height = _rect.Height;
		//基于屏幕的中心点
		_rect.X = x + (sw - width) / 2.0f + 0.5;//保证左右居中
		_rect.Y = y + (sh - height) / 2.0f + 0.5;//保证上下居中
		//移动窗口
		::SetWindowPos(Hwnd(), NULL, _rect.X, _rect.Y, _rect.Width, _rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Window::CenterToWindow(HWND wnd)
	{
		if (wnd == NULL) {
			wnd = ::GetWindow(Hwnd(), GW_OWNER);
		}
		if (wnd == NULL) {
			wnd = ::GetParent(Hwnd());
		}
		if (wnd == NULL) {
			return;
		}
		Rect _rect = this->GetWindowRect();
		const int& width = _rect.Width;
		const int& height = _rect.Height;
		//基于父窗口的中心点
		RECT ownerRECT;
		::GetWindowRect(wnd, &ownerRECT);
		int onwerWidth = ownerRECT.right - ownerRECT.left;
		int onwerHeight = ownerRECT.bottom - ownerRECT.top;
		if (width > 0 && height > 0 && onwerWidth > 0 && onwerHeight > 0) {
			_rect.X = ownerRECT.left + (onwerWidth - width) / 2.0f + 0.5;
			_rect.Y = ownerRECT.top + (onwerHeight - height) / 2.0f + 0.5;
			//移动窗口
			::SetWindowPos(Hwnd(), NULL, _rect.X, _rect.Y, _rect.Width, _rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Window::SetFocus(Control* ctl)
	{
		if (ctl == NULL) {
			return;
		}
		if (_inputControl) {
			KillFocusEventArgs args(ctl);
			_inputControl->DispatchEvent(args);
		}
		FocusEventArgs args(_inputControl);
		ctl->DispatchEvent(args);
		_inputControl = ctl;
		_focusControl = ctl;
	}

	LRESULT  Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//if (WM_COMMAND == uMsg) {
		//	//windows子控件消息
		//}
		switch (uMsg)
		{
		case WM_SETFOCUS:
		{
			this->OnFocus((HWND)wParam);
			break;
		}
		case WM_KILLFOCUS:
		{
			this->OnKillFocus((HWND)wParam);
			break;
		}
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
				HIMC hIMC = ImmGetContext(Hwnd());
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
				ImmReleaseContext(Hwnd(), hIMC);
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
			break;
		}
		case WM_DPICHANGED:
		{
			int dpi = HIWORD(wParam);
			//新的缩放比
			FLOAT systemScale = (float)dpi / USER_DEFAULT_SCREEN_DPI;
			RECT* const prcNewWindow = (RECT*)lParam;
			int newX = prcNewWindow->left;
			int newY = prcNewWindow->top;
			int newWidth = prcNewWindow->right - prcNewWindow->left;
			int newHeight = prcNewWindow->bottom - prcNewWindow->top;
			this->OnDpiChange(systemScale, Rect(newX, newY, newWidth, newHeight));
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
			//NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);
			//if (pNMHDR->hwndFrom == _hWndTips && pNMHDR->code==) {
			//	NMTTDISPINFO* pDispInfo = reinterpret_cast<NMTTDISPINFO*>(lParam);
			//	// 在这里更改提示文字
			//	pDispInfo->szText = const_cast<TCHAR*>(L"新的提示文字");
			//	return true;
			//}
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
				//解决关闭窗口时 owner窗口闪烁的问题
				if (_oWnerWnd) {
					::EnableWindow(_oWnerWnd, TRUE);
				}
				::DestroyWindow(Hwnd());
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
#ifdef _DEBUG
			if (wParam == VK_F11) {
				PublicData->Debug = !PublicData->Debug;
				if (PublicData->Debug) {
					if (PublicData->ColorIndex >= PublicData->DebugColors.size()) {
						PublicData->ColorIndex = 0;
					}
					PublicData->DebugColors;
					PublicData->DebugColor = PublicData->DebugColors[PublicData->ColorIndex];
					PublicData->ColorIndex++;
				}
				Invalidate();
			}
#endif
			OnKeyDown(wParam, lParam);
			break;
		}
		case WM_KEYUP: {
			OnKeyUp(wParam, lParam);
			break;
		}
		case WM_DESTROY:
		{
			this->PublicData->HANDLE = NULL;
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
					Control* pCtl = outCtl;
					if ((pCtl->EventPassThrough & Event::OnHover)) {
						while (pCtl)
						{
							auto cursor = pCtl->GetHCursor();
							if (cursor) {
								::SetCursor(cursor);
								return TRUE;
							}
							pCtl = pCtl->Parent;
						}
					}
					else {
						auto cursor = pCtl->GetHCursor();
						if (cursor) {
							::SetCursor(cursor);
							return TRUE;
						}
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
			//给hwndTip发送消息告诉现在移动到什么位置了
			//LPARAM lp = MAKELPARAM(args.Location.X, args.Location.Y);
			//SendMessage(_hWndTips, TTM_TRACKPOSITION, 0, lParam);
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
		return ::DefWindowProc(Hwnd(), uMsg, wParam, lParam);
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
		args.PublicData = this->PublicData;
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
		if (_layout) {
			_layout->DispatchEvent(arg);//
		}
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
		Control* outCtl = _layout;
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

		const Controls* pTemp;
		if (outCtl->GetViewControls().size() > 0) {
			pTemp = &outCtl->GetViewControls();
		}
		else {
			pTemp = (Controls*)(&(outCtl->GetControls()));
		}

		for (auto itor = pTemp->rbegin(); itor != pTemp->rend(); ++itor) {
			Control& it = **itor;
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
		//如果控件是弹簧的情况下直接穿透
		if (dynamic_cast<Spacer*>(outCtl) && outCtl->Parent) {
			return  outCtl->Parent;
		}
		////鼠标键盘的事件是可以穿透的(这样做貌似不是很好)
		//if ((outCtl->EventPassThrough & Event::OnMouseEvent || outCtl->EventPassThrough & Event::OnKeyBoardEvent) && outCtl->Parent) {
		//	return outCtl->Parent;
		//}
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
				MouseEventArgs args(Event::OnMouseLeave);
				auto rect = _focusControl->GetClientRect();
				args.Location.X = point.X - rect.X;
				args.Location.Y = point.Y - rect.Y;
				ok = _focusControl->DispatchEvent(args);
			}
			//触发MouseEnter
			args.EventType = Event::OnMouseEnter;
			ok = newCtl->DispatchEvent(args);
			if (ok) {
				_focusControl = newCtl;
			}
		}
		//触发命中的MouseMove
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

	void Window::OnMouseWheel(int zDelta, const Point& point)
	{
		if (_focusControl == NULL) return;
		if (_focusControl) {
			MouseEventArgs args(Event::OnMouseWheel);
			args.Location = point;
			args.ZDelta = zDelta;
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
			args.Location = point;
			args.ZDelta = zDelta;
			scrollBar->DispatchEvent(args);
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
		::SetCapture(Hwnd());
		_mouseDown = true;

		//记录鼠标按下的时间
		auto _time = std::chrono::system_clock::now();
		//_lastDownTime = _time;

		//寻早控件
		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);
		//如果单机的不是上一个 那么上一个触发失去焦点事件
		if (_inputControl != outCtl) {
			this->SetFocus(outCtl);//设置焦点
		}
		//给命中的控件触发鼠标按下事件
		if (_inputControl) {
			MouseEventArgs args(Event::OnMouseDown);
			args.Button = mbtn;
			args.Location = relativePoint;
			_inputControl->DispatchEvent(args);
		}
		//做双击消息处理
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
			//触发抬起事件
			{
				args.EventType = Event::OnMouseUp;
				_inputControl->DispatchEvent(args);

			}
			//触发单击事件 如果焦点还在并且鼠标未移出控件内 
			if (_inputControl && mbtn == _lastBtn && ctlRect.Contains(point)) {
				//auto _time = std::chrono::system_clock::now();
				//auto diff = _time - _lastDownTime;
				//auto timeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
				//if (timeOffset < 100) {//鼠标按住超过200毫秒则不触发单机事件
				//	
				//}
				args.EventType = Event::OnMouseClick;
				_inputControl->DispatchEvent(args);
			}
			if (_inputControl) {
				POINT p1{ 0 };
				::GetCursorPos(&p1);
				::ScreenToClient(Hwnd(), &p1);
				if (ctlRect.Contains(p1.x, p1.y) && ::GetForegroundWindow() == Hwnd()) {
					args.EventType = Event::OnMouseEnter;//触发鼠标悬浮事件
				}
				else {
					args.EventType = Event::OnMouseLeave;//触发鼠标离开事件
				}
				_inputControl->DispatchEvent(args);
			}
		}
	}

	void Window::OnMouseClick(MouseButton mbtn, const Point& point) {

	}

	const float& Window::GetScale() {
		return this->PublicData->Scale;
	}

	void Window::OnSize(const Size& sz)
	{
		if (!_layout) {
			return;
		}
		if (_layout->GetScale() != PublicData->Scale) {
			this->OnDpiChange(PublicData->Scale, Rect());
		}
		_layout->SetRect(this->GetClientRect());
		_layout->Invalidate();
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
	void Window::OnFocus(HWND hWnd)
	{
	}
	void Window::OnLocation(const Point& point) {
	}
	void Window::OnKillFocus(HWND hWnd)
	{
	}

	void Window::MoveWindow() {
		::ReleaseCapture();
		SendMessage(Hwnd(), WM_NCLBUTTONDOWN, HTCAPTION, NULL);//模拟鼠标按住标题栏移动窗口
		SendMessage(Hwnd(), WM_LBUTTONUP, NULL, NULL);//松开
	}

	void Window::OnDpiChange(const float& systemScale, const Rect& newRect)
	{
		//新的缩放比
		float newScale = systemScale / PublicData->Scale;
		this->PublicData->Scale = systemScale;
		this->_miniSize.Scale(newScale);
		this->_maxSize.Scale(newScale);
		DpiChangeEventArgs arg(systemScale);
		if (_layout) {
			_layout->DispatchEvent(arg);
		}
		if (!newRect.IsEmptyArea()) {
			SetWindowPos(Hwnd(), NULL, newRect.X, newRect.Y, newRect.Width, newRect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	bool Window::OnNotify(Control* sender, EventArgs& args) {
		if (args.EventType == Event::OnMouseDoubleClick) {
			if (sender->Action == ControlAction::TitleBar) {
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
			if (sender->Action == ControlAction::TitleBar || sender->Action == ControlAction::MoveWindow) {
				MoveWindow();
				return false;
			}
			if (sender->Action == ControlAction::Mini) {
				this->ShowMinimized();
				return false;
			}
			if (sender->Action == ControlAction::Max) {
				if (!IsZoomed(Hwnd())) {
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
							tabLayout->Invalidate();
							break;
						}
						++pos;
					}
				}
			}

		}
		return false;
	}
};