#include "Window.h"
#include "TabLayout.h"
#include "IFrame.h"
#include <CommCtrl.h>

namespace ezui {

	Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
	{
		Init(width, height, owner, dStyle, ExStyle);//设置基本数据
	}

	Window::~Window()
	{
		if (Hwnd()) {
			::DestroyWindow(Hwnd());
		}
		if (m_publicData) {
			delete m_publicData;
		}
	}

	void Window::Init(int width, int height, HWND owner, DWORD dStyle, DWORD  exStyle)
	{
		this->m_publicData = new WindowData;
		Rect rect(0, 0, width, height);

		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		for (auto& it : ezui::__EzUI__MonitorInfos) {
			Rect rect = it.Rect;
			rect.Width = it.Physical.Width;
			rect.Height = it.Physical.Height;
			if (rect.Contains(cursorPos.x, cursorPos.y)) {
				rect.X = rect.X;
				rect.Y = rect.Y;
				this->m_publicData->Scale = it.Scale;
				break;
			}
		}

		rect.Scale(this->m_publicData->Scale);

		//绑定消息过程
		m_publicData->WndProc = [this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ->LRESULT {
			return this->WndProc(uMsg, wParam, lParam);
			};
		//创建窗口
		HWND hWnd = ::CreateWindowExW(exStyle | WS_EX_ACCEPTFILES, EZUI_WINDOW_CLASS, EZUI_WINDOW_CLASS, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dStyle,
			rect.X, rect.Y, rect.Width, rect.Height, owner, NULL, ezui::__EzUI__HINSTANCE, NULL);
		//设置窗口句柄
		this->SetHwnd(hWnd);
		m_publicData->Window = this;

		if (owner) {
			this->CenterToWindow(owner);
		}
		else {
			this->CenterToScreen();
		}

		if ((exStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			m_publicData->InvalidateRect = [this](const Rect& rect)->void {
				RECT r;
				r.left = rect.GetLeft();
				r.top = rect.GetTop();
				r.right = rect.GetRight();
				r.bottom = rect.GetBottom();
				::InvalidateRect(Hwnd(), &r, FALSE);
				};
			m_publicData->UpdateWindow = [this]()->void {
				RECT updateRect;
				while (::GetUpdateRect(Hwnd(), &updateRect, FALSE))
				{
					Rect rect(updateRect);
					if (!rect.IsEmptyArea()) {
						::UpdateWindow(Hwnd());
					}
				}
				};
		}
		m_publicData->CleanControl = [this](Control* delControl)->void {
			if (m_focusControl == delControl) {
				m_focusControl = NULL;
			}
			if (m_inputControl == delControl) {
				m_inputControl = NULL;
			}
			};
		m_publicData->SendNotify = [this](Control* sender, EventArgs& args)->bool {
			IFrame* frame = NULL;
			Control* parent = sender;
			//依次往上父控件看看有没有当前控件是否在内联页面中
			while (parent)
			{
				if (frame = dynamic_cast<IFrame*>(parent)) {
					break;
				}
				parent = parent->Parent;
			}
			//如果当前控件存在与内联界面且事件通知处理器不为NULL的时候
			if (frame) {
				if (frame->OnNotify(sender, args)) {
					return true;
				}
				if (frame->EventHandler) {
					frame->EventHandler(sender, args);
				}
			}
			return this->OnNotify(sender, args);
			};

		//绑定窗口的数据
		UI_SET_USERDATA(Hwnd(), this->m_publicData);

		//触发一些逻辑
		::SendMessage(Hwnd(), WM_MOVE, NULL, MAKELPARAM(rect.X, rect.Y));
		::SendMessage(Hwnd(), WM_SIZE, NULL, MAKELPARAM(rect.Width, rect.Height));
	}

	Control* Window::FindControl(const UIString& objectName)
	{
		if (!m_layout) {
			return NULL;
		}
		return this->m_layout->FindControl(objectName);
	}

	int Window::X() {
		return GetWindowRect().X;
	}
	int Window::Y() {
		return GetWindowRect().Y;
	}
	int Window::Width() {
		return GetWindowRect().Width;
	}
	int Window::Height() {
		return GetWindowRect().Height;
	}

	const Rect& Window::GetWindowRect()
	{
		RECT rect;
		::GetWindowRect(Hwnd(), &rect);
		m_rect = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
		return m_rect;
	}
	const Rect& Window::GetClientRect()
	{
		RECT rect;
		::GetClientRect(Hwnd(), &rect);
		m_rectClient = { rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top };
		return m_rectClient;
	}

	void Window::SetText(const UIString& text)
	{
		::SetWindowTextW(Hwnd(), text.unicode().c_str());
	}

	UIString Window::GetText() {
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
	bool Window::IsFullScreen() {
		//判断是否全屏
		MonitorInfo monitorInfo;
		ezui::GetMontior(&monitorInfo, Hwnd());//获取窗口所在的监视器位置
		auto rect = this->GetWindowRect();
		bool isFullScreen = (monitorInfo.Rect.X == rect.X && monitorInfo.Rect.Y == rect.Y && monitorInfo.Rect.Width == rect.Width && monitorInfo.Rect.Height == rect.Height);
		if (isFullScreen)
		{
			return true;
		}
		return false;
	}

	bool Window::IsMinimized()
	{
		return ::IsIconic(Hwnd());
	}
	bool Window::IsMaximized() {
		return ::IsZoomed(Hwnd());
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
		::MoveWindow(Hwnd(), rect.X, rect.Y, rect.Width, rect.Height, TRUE);
	}
	void Window::SetMiniSize(const Size& size)
	{
		m_miniSize = size;
		m_miniSize.Scale(this->m_publicData->Scale);
	}
	void Window::SetMaxSize(const Size& size)
	{
		m_maxSize = size;
		m_maxSize.Scale(this->m_publicData->Scale);
	}
	void Window::SetIcon(short id)
	{
		SetIcon(::LoadIcon(::ezui::__EzUI__HINSTANCE, MAKEINTRESOURCE(id)));//
	}
	void Window::SetIcon(HICON icon)
	{
		::SendMessage(Hwnd(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
	}
	void Window::SetLayout(ezui::Control* layout) {
		ASSERT(layout);
		m_layout = layout;
		m_layout->SetHwnd(Hwnd());

		if (m_layout->Style.FontFamily.empty()) {
			NONCLIENTMETRICS ncm = {};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0)) {
				//LOGFONTW lf = ncm.lfMessageFont;
				//HFONT hFont = CreateFontIndirectW(&lf);
				//lf.lfFaceName 就是系统 UI 默认字体名
				// hFont 就是可以直接用的字体句柄
				m_layout->Style.FontFamily = ncm.lfMessageFont.lfFaceName;
			}
		}
		if (m_layout->Style.FontSize == 0) {
			m_layout->Style.FontSize = 12;
		}
		if (m_layout->Style.ForeColor.GetValue() == 0) {
			m_layout->Style.ForeColor = Color::Black;
		}
		if (m_layout->Style.BackColor.GetValue() == 0) {
			LONG_PTR exStyle = ::GetWindowLongPtr(Hwnd(), GWL_EXSTYLE);
			bool isLayered = ((exStyle & WS_EX_LAYERED) != 0);
			if (!isLayered) {//非layered窗口 主布局控件必须设置背景色
				m_layout->Style.BackColor = Color::White;
			}
		}
		if (m_layout->GetScale() != this->GetScale()) {
			this->DispatchEvent(m_layout, DpiChangeEventArgs(this->GetScale()));
		}
		m_layout->SetRect(this->GetClientRect());
	}

	Control* Window::GetLayout()
	{
		return this->m_layout;
	}
	void Window::Close(int code) {
		m_closeCode = code;
		::SendMessage(Hwnd(), WM_CLOSE, 0, 0);
	}
	void Window::Destroy()
	{
		::DestroyWindow(Hwnd());
	}
	void Window::Show()
	{
		ASSERT(m_layout);
		::ShowWindow(Hwnd(), SW_SHOW);
	}
	void Window::Show(int cmdShow)
	{
		::ShowWindow(Hwnd(), cmdShow);
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
		ezui::GetMontior(&monitorInfo, Hwnd());
		SetRect(monitorInfo.Rect);
		SetTopMost(true);
		::ShowWindow(Hwnd(), SW_SHOW);
	}
	int Window::ShowModal(bool disableOwner)
	{
		//此处代码不能随意更改 解决关闭窗口时,owner窗口闪烁问题
		if (disableOwner) {
			m_ownerWnd = ::GetWindow(Hwnd(), GW_OWNER);
		}
		if (m_ownerWnd) {
			::EnableWindow(m_ownerWnd, FALSE);
		}
		this->Show();
		::MSG msg{ 0 };
		while (Hwnd() && ::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (m_ownerWnd && ::IsWindow(m_ownerWnd)) {
			::SetActiveWindow(m_ownerWnd);
			m_ownerWnd = NULL;
		}
		return m_closeCode;
	}
	void Window::Hide() {
		this->Refresh();//隐藏窗口前先刷新一下窗口中的画面(清除残留画面)
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
		m_publicData->InvalidateRect(this->GetClientRect());
	}
	void Window::Refresh()
	{
		m_publicData->UpdateWindow();
	}

	void Window::CenterToScreen()
	{
		MonitorInfo monitorInfo;
		ezui::GetMontior(&monitorInfo);

		int x = monitorInfo.WorkRect.X;
		int y = monitorInfo.WorkRect.Y;
		int sw = monitorInfo.WorkRect.Width;//当前工作区域的宽
		int sh = monitorInfo.WorkRect.Height;//当前工作区域的高

		Rect rect = this->GetWindowRect();
		int width = rect.Width;
		int height = rect.Height;
		//基于屏幕的中心点
		rect.X = x + (sw - width) / 2.0f + 0.5;//保证左右居中
		rect.Y = y + (sh - height) / 2.0f + 0.5;//保证上下居中
		//移动窗口
		SetRect({ rect.X, rect.Y, rect.Width, rect.Height });
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
		Rect rect = this->GetWindowRect();
		const int& width = rect.Width;
		const int& height = rect.Height;
		//基于父窗口的中心点
		RECT ownerRECT;
		::GetWindowRect(wnd, &ownerRECT);
		int onwerWidth = ownerRECT.right - ownerRECT.left;
		int onwerHeight = ownerRECT.bottom - ownerRECT.top;
		if (width > 0 && height > 0 && onwerWidth > 0 && onwerHeight > 0) {
			rect.X = ownerRECT.left + (onwerWidth - width) / 2.0f + 0.5;
			rect.Y = ownerRECT.top + (onwerHeight - height) / 2.0f + 0.5;
			//移动窗口
			SetRect({ rect.X, rect.Y, rect.Width, rect.Height });
		}
	}

	void Window::SetFocus(Control* ctl)
	{
		if (ctl == NULL) {
			return;
		}
		if (m_inputControl) {
			KillFocusEventArgs args(ctl);
			this->DispatchEvent(m_inputControl, args);
		}
		FocusEventArgs args(m_inputControl);
		this->DispatchEvent(ctl, args);
		m_inputControl = ctl;
		m_focusControl = ctl;
	}

	WindowData* Window::GetPublicData()
	{
		return m_publicData;
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
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pMMInfo = (MINMAXINFO*)lParam;
			if (!m_miniSize.Empty()) {
				pMMInfo->ptMinTrackSize.x = m_miniSize.Width;
				pMMInfo->ptMinTrackSize.y = m_miniSize.Height;
			}
			if (!m_maxSize.Empty()) {
				pMMInfo->ptMaxTrackSize.x = m_maxSize.Width;
				pMMInfo->ptMaxTrackSize.y = m_maxSize.Height;
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
		case WM_CHAR: {
			OnKeyChar(wParam, lParam);
			break;
		}
		case  WM_IME_STARTCOMPOSITION://
		{
			if (m_inputControl) {
				HIMC hIMC = ImmGetContext(Hwnd());
				Rect rect = m_inputControl->GetCareRect();
				Rect inputRect = m_inputControl->GetClientRect();
				int	x = inputRect.X + rect.X;
				int y = inputRect.Y + rect.Y;

				COMPOSITIONFORM cpf = {};
				cpf.dwStyle = CFS_POINT;
				cpf.ptCurrentPos.x = x;
				cpf.ptCurrentPos.y = y;
				ImmSetCompositionWindow(hIMC, &cpf);
				ImmReleaseContext(Hwnd(), hIMC);
			}
			break;
		}
		case WM_IME_COMPOSITION:
		{
			if (lParam & GCS_RESULTSTR) {
				HIMC hImc = ImmGetContext(Hwnd());
				if (hImc) {
					LONG bytes = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, nullptr, 0);
					if (bytes > 0) {
						std::wstring committed;
						committed.resize(bytes / sizeof(wchar_t) + 1);
						ImmGetCompositionStringW(hImc, GCS_RESULTSTR, (WCHAR*)committed.data(), bytes);
						for (auto& it : committed) {
							OnKeyChar((WPARAM)it, NULL);
						}
					}
					ImmReleaseContext(Hwnd(), hImc);
					return 0;
				}
			}
			break;
		}
		case WM_IME_ENDCOMPOSITION: {
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
		case WM_SHOWWINDOW: {
			break;
		}
		case WM_WINDOWPOSCHANGED: {
			WINDOWPOS* wPos = (WINDOWPOS*)(void*)lParam;
			if ((wPos->flags & SWP_NOCOPYBITS) == SWP_NOCOPYBITS) {
				//丢弃工作区的整个内容。 如果未指定此标志，则会在调整或重新定位窗口后保存并复制回工作区的有效内容。
				this->Invalidate();
			}
			if ((wPos->flags & SWP_SHOWWINDOW) == SWP_SHOWWINDOW) {
				this->Invalidate();
			}
			break;
		}
		case WM_MOVE: {
			if (IsMinimized()) {
				//窗口最小化的时候不做处理
				break;
			}
			int xPos = (int)(short)LOWORD(lParam);   // horizontal position 
			int yPos = (int)(short)HIWORD(lParam);   // vertical position 
			m_rect.X = xPos;
			m_rect.Y = yPos;

			OnMove(m_rect.GetLocation());
			break;
		}
		case WM_SIZE: {
			if (IsMinimized()) {
				//窗口最小化的时候不做处理
				break;
			}
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			m_rect.Width = width;
			m_rect.Height = height;

			//获取客户区的矩形
			RECT rect;
			::GetClientRect(Hwnd(), &rect);

			m_rectClient.X = rect.left;
			m_rectClient.Y = rect.top;
			m_rectClient.Width = rect.right - rect.left;
			m_rectClient.Height = rect.bottom - rect.top;

			OnSize(m_rect.GetSize());
			break;
		}
		case WM_CLOSE:
		{
			bool bClose = true;
			OnClose(bClose);
			if (bClose) {
				//解决关闭窗口时 owner窗口闪烁的问题
				if (m_ownerWnd) {
					::EnableWindow(m_ownerWnd, TRUE);
				}
				::DestroyWindow(Hwnd());
			}
			else {
				//关闭已取消
				return TRUE;
			}
			break;
		}
		case WM_KEYDOWN:
		{
#ifdef _DEBUG
			if (wParam == VK_F11) {
				m_publicData->Debug = !m_publicData->Debug;
				if (m_publicData->Debug) {
					m_publicData->DebugColor = m_publicData->DebugColors[m_publicData->ColorIndex];
					m_publicData->ColorIndex++;
					if (m_publicData->ColorIndex >= m_publicData->DebugColors.size()) {
						m_publicData->ColorIndex = 0;
					}
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
			SetHwnd(NULL);
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
				Control* outCtl = this->HitTestControl(point, &relativePoint);//找到当前控件的位置
				if (outCtl && outCtl->IsEnabled()) {
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
		case WM_MOUSEHOVER: {
			//鼠标悬停消息
			m_bTracking = false;
			auto xPos = GET_X_LPARAM(lParam);
			auto yPos = GET_Y_LPARAM(lParam);
			OnMouseHover({ xPos,yPos });
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (!m_bTracking) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.hwndTrack = Hwnd();
				tme.dwHoverTime = 500;//500毫秒算鼠标悬停
				if (TrackMouseEvent(&tme) == TRUE) {
					m_bTracking = true;
				}
			}
			auto xPos = GET_X_LPARAM(lParam);
			auto yPos = GET_Y_LPARAM(lParam);
			//OutputDebugStringA(UIString("%d %d\n").format(x, y).c_str());
			OnMouseMove({ xPos,yPos });
			m_mouseIn = true;
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
			m_bTracking = false;
			m_mouseIn = false;
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
		auto t1 = ::GetTickCount64();
#endif // COUNT_ONPAINT
#if USED_DIRECT2D
		DXRender graphics(winHDC, 0, 0, GetClientRect().Width, GetClientRect().Height);
		PaintEventArgs args(graphics);
		args.DC = winHDC;
		args.HWND = Hwnd();
		args.PublicData = this->m_publicData;
		args.PublicData->PaintCount = 0;
		args.InvalidRectangle = rePaintRect;
		OnPaint(args);
#endif
#if COUNT_ONPAINT
		char buf[512]{ 0 };
		sprintf(buf, "Time:%ld DC:%p OnPaint Count(%d) Rect(%d,%d,%d,%d) %dms \n", ::GetTickCount64(), winHDC, args.PublicData->PaintCount, rePaintRect.X, rePaintRect.Y, rePaintRect.Width, rePaintRect.Height, ::GetTickCount64() - t1);
		OutputDebugStringA(buf);
#endif // COUNT_ONPAINT

	}

	void Window::OnPaint(PaintEventArgs& arg)
	{
		if (m_layout) {
			this->DispatchEvent(m_layout, arg);
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

	Control* Window::HitTestControl(const Point clientPoint, Point* outPoint) {
		*outPoint = clientPoint;
		Control* outCtl = m_layout;
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
		//鼠标键盘的事件是可以穿透的(这样做貌似不是很好)
	/*	if ((outCtl->EventPassThrough & Event::OnMouseEvent || outCtl->EventPassThrough & Event::OnKeyBoardEvent) && outCtl->Parent) {
			return outCtl->Parent;
		}*/
		return outCtl;
	}

	bool Window::DispatchEvent(Control* ctrl, const EventArgs& args)
	{
		if (ctrl) {
			return ctrl->SendEvent(args);
		}
		return false;
	}

	void Window::OnMouseHover(const Point& point) {

	}

	void Window::OnMouseMove(const Point& point)
	{
		//对窗口进行移动
		if (m_moveWindow && m_mouseDown) {
			POINT ptNow;
			::GetCursorPos(&ptNow);
			int dx = ptNow.x - m_dragPoint.x;
			int dy = ptNow.y - m_dragPoint.y;
			HWND hWnd = Hwnd();
			RECT rect;
			::GetWindowRect(hWnd, &rect);
			this->SetRect(Rect(rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top));
			m_dragPoint = ptNow; // 更新起点
			return;
		}

		if (m_inputControl && m_mouseDown) { //按住移动的控件
			auto ctlRect = m_inputControl->GetClientRect();
			MouseEventArgs args(Event::OnMouseMove, { point.X - ctlRect.X ,point.Y - ctlRect.Y });
			this->DispatchEvent(m_inputControl, args);
			return;
		}

		Point relativePoint;
		Control* newCtl = this->HitTestControl(point, &relativePoint);//找到当前控件的位置
		MouseEventArgs args(Event::None);
		args.Location = relativePoint;

		//触发上一个
		if (m_focusControl != newCtl) {
			bool ok = true;
			if (m_focusControl) {
				MouseEventArgs args(Event::OnMouseLeave);
				auto rect = m_focusControl->GetClientRect();
				args.Location.X = point.X - rect.X;
				args.Location.Y = point.Y - rect.Y;
				ok = this->DispatchEvent(m_focusControl, args);
			}
			//触发MouseEnter
			args.EventType = Event::OnMouseEnter;
			ok = this->DispatchEvent(newCtl, args);
			if (ok) {
				m_focusControl = newCtl;
			}
		}
		//触发命中的MouseMove
		if (m_focusControl) {
			args.EventType = Event::OnMouseMove;
			this->DispatchEvent(m_focusControl, args);
		}

	}
	void Window::OnMouseLeave()
	{
		if (m_focusControl) {
			MouseEventArgs args(Event::OnMouseLeave);
			this->DispatchEvent(m_focusControl, args);
		}
		m_focusControl = NULL;
		m_mouseDown = false;
	}

	void Window::OnMouseDoubleClick(MouseButton mbtn, const Point& point)
	{
		Point relativePoint;
		Control* outCtl = this->HitTestControl(point, &relativePoint);
		if (outCtl) {
			MouseEventArgs args(Event::OnMouseDoubleClick);
			args.Button = mbtn;
			args.Location = relativePoint;
			this->DispatchEvent(outCtl, args);
		}
	}

	void Window::OnMouseDown(MouseButton mbtn, const Point& point)
	{
		::SetCapture(Hwnd());
		m_mouseDown = true;//标记为按下
		//寻早控件
		Point relativePoint;
		Control* outCtl = this->HitTestControl(point, &relativePoint);
		//如果单机的不是上一个 那么上一个触发失去焦点事件
		if (m_inputControl != outCtl) {
			this->SetFocus(outCtl);//设置焦点
		}
		//给命中的控件触发鼠标按下事件
		if (m_inputControl) {
			MouseEventArgs args(Event::OnMouseDown);
			args.Button = mbtn;
			args.Location = relativePoint;
			this->DispatchEvent(m_inputControl, args);
		}
		//获取按下按钮和按下的时差
		auto time_now = ::GetTickCount64();//记录鼠标按下的当前时间
		auto offset = time_now - m_lastDownTime;//本次与上次按下按钮的时间差
		//300毫秒之内同一个按钮按下两次算双击消息
		if (offset < 300 && m_lastBtn == mbtn && m_downPoint == point) {
			OnMouseDoubleClick(mbtn, point);
		}
		m_lastDownTime = ::GetTickCount64();
		m_lastBtn = mbtn;
		m_downPoint = point;
	}
	void Window::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		m_moveWindow = false;
		::ReleaseCapture();
		if (m_mouseDown == false) {
			return;
		}
		m_mouseDown = false;
		if (m_inputControl) {
			Rect ctlRect = m_inputControl->GetClientRect();
			MouseEventArgs args(Event::None);
			args.Button = mbtn;
			args.Location = { point.X - ctlRect.X,point.Y - ctlRect.Y };
			//触发抬起事件
			{
				args.EventType = Event::OnMouseUp;
				this->DispatchEvent(m_inputControl, args);

			}
			if (m_inputControl) {
				POINT p1{ 0 };
				::GetCursorPos(&p1);
				::ScreenToClient(Hwnd(), &p1);
				if (ctlRect.Contains(p1.x, p1.y) && ::GetForegroundWindow() == Hwnd()) {
					args.EventType = Event::OnMouseEnter;//触发鼠标悬浮事件
				}
				else {
					args.EventType = Event::OnMouseLeave;//触发鼠标离开事件
				}
				this->DispatchEvent(m_inputControl, args);
			}
		}
	}
	void Window::OnMouseWheel(int zDelta, const Point& point)
	{
		if (m_focusControl == NULL) return;
		if (m_focusControl) {
			MouseEventArgs args(Event::OnMouseWheel);
			args.Location = point;
			args.ZDelta = zDelta;
			this->DispatchEvent(m_focusControl, args);
		}
		ScrollBar* scrollBar = NULL;
		if (m_focusControl && m_focusControl->GetScrollBar() && m_focusControl->GetScrollBar()->Scrollable()) {
			scrollBar = dynamic_cast<ScrollBar*>(m_focusControl->GetScrollBar());
		}
		Control* pControl = m_focusControl;
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
			this->DispatchEvent(scrollBar, args);
		}
	}

	float Window::GetScale() {
		return this->m_publicData->Scale;
	}

	void Window::OnSize(const Size& sz)
	{
		if (!m_layout) {
			return;
		}
		if (m_layout->GetScale() != m_publicData->Scale) {
			this->OnDpiChange(m_publicData->Scale, Rect());
		}
		m_layout->SetRect(this->GetClientRect());
	}

	void Window::OnClose(bool& bClose)
	{

	}
	void Window::OnDestroy()
	{

	}
	void Window::OnKeyChar(WPARAM wParam, LPARAM lParam)
	{
		if (m_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyChar, wParam, lParam);
			this->DispatchEvent(m_inputControl, args);
			return;
		}
	}
	void Window::OnKeyDown(WPARAM wParam, LPARAM lParam) {
		if (m_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyDown, wParam, lParam);
			this->DispatchEvent(m_inputControl, args);
			return;
		}
	}
	void Window::OnKeyUp(WPARAM wParam, LPARAM lParam) {
		if (m_inputControl) { //
			KeyboardEventArgs args(Event::OnKeyUp, wParam, lParam);
			this->DispatchEvent(m_inputControl, args);
			return;
		}
	}
	void Window::OnFocus(HWND hWnd)
	{
	}
	void Window::OnMove(const Point& point) {
	}
	void Window::OnKillFocus(HWND hWnd)
	{
		if (m_inputControl) { //窗口失去焦点
			KillFocusEventArgs args(NULL);
			this->DispatchEvent(m_inputControl, args);
			m_inputControl = NULL;
		}
	}

	bool Window::IsWindow()const {
		return true;
	}
	void Window::TitleMoveWindow() {
		::ReleaseCapture();
		::SendMessage(Hwnd(), WM_NCLBUTTONDOWN, HTCAPTION, NULL);//模拟鼠标按住标题栏移动窗口,会吃掉鼠标左键的弹起消息,手动触发也无惧于事
		::SendMessage(Hwnd(), WM_LBUTTONUP, NULL, NULL);//松开 不调发送此消息会导致一些消息问题
	}

	void Window::OnDpiChange(float systemScale, const Rect& newRect)
	{
		//新的缩放比
		float newScale = systemScale / m_publicData->Scale;
		this->m_publicData->Scale = systemScale;
		this->m_miniSize.Scale(newScale);
		this->m_maxSize.Scale(newScale);
		DpiChangeEventArgs arg(systemScale);
		if (m_layout) {
			this->DispatchEvent(m_layout, arg);
		}
		if (!newRect.IsEmptyArea()) {
			SetRect({ newRect.X, newRect.Y, newRect.Width, newRect.Height });
		}
	}
	bool Window::OnNotify(Control* sender, EventArgs& args) {

		switch (args.EventType)
		{
		case Event::OnMouseDoubleClick: {
			if (sender->Action == ControlAction::Title) {
				if (::IsZoomed(Hwnd())) {
					this->ShowNormal();
				}
				else {
					this->ShowMaximized();
				}
			}
			break;
		}
		case Event::OnMouseDown: {
			if (sender->Action == ControlAction::MoveWindow) {
				GetCursorPos(&m_dragPoint); // 获取屏幕坐标
				m_moveWindow = true;
				break;
			}
			if (sender->Action == ControlAction::Title) {
				TitleMoveWindow();
				break;
			}
			if (sender->Action == ControlAction::Mini) {
				this->ShowMinimized();
				break;
			}
			if (sender->Action == ControlAction::Max) {
				if (!IsMaximized()) {
					this->ShowMaximized();
				}
				else {
					this->ShowNormal();
				}
				break;
			}
			if (sender->Action == ControlAction::Close) {
				MouseEventArgs args(Event::OnMouseLeave);
				this->DispatchEvent(sender, args);
				this->Close();
				break;
			}
			UIString tabName = sender->GetAttribute("tablayout");
			if (!tabName.empty()) {
				auto ctls = sender->Parent->FindControl("tablayout", tabName);
				TabLayout* tabLayout = dynamic_cast<TabLayout*>(FindControl(tabName));
				if (tabLayout && sender->Parent) {
					int pos = 0;
					for (auto& it : ctls)
					{
						if (it == sender) {
							tabLayout->SlideToPage(pos);
							tabLayout->Invalidate();
							break;
						}
						++pos;
					}
				}
			}
			break;
		}
		default:
			break;
		}
		return false;
	}
};