#include "Window.h"
#include "TabLayout.h"

namespace EzUI {
#define _focusControl PublicData.FocusControl
#define _inputControl PublicData.InputControl

	Window::Window(int width, int height, HWND owner, DWORD dStyle, DWORD  ExStyle)
	{
		InitWindow(width, height, owner, dStyle, ExStyle);//���û�������
	}

	Window::~Window()
	{
		if (_hWndTips) {
			::DestroyWindow(_hWndTips);
		}
		if (Hwnd()) {
			::DestroyWindow(Hwnd());
		}
	}

	void Window::InitWindow(int width, int height, HWND owner, DWORD dStyle, DWORD  exStyle)
	{
		float scanle = 1.0f;
		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		for (auto& it : EzUI::Base::MonitorInfos) {
			Rect rect = it.Rect;
			rect.Width = it.Physical.Width;
			rect.Height = it.Physical.Height;
			if (rect.Contains(cursorPos.x, cursorPos.y)) {
				scanle = it.Scale;
				break;
			}
		}

		MonitorInfo monitorInfo;
		EzUI::GetMontior(&monitorInfo);

		int x = monitorInfo.WorkRect.X;
		int y = monitorInfo.WorkRect.Y;
		int sw = monitorInfo.WorkRect.Width;//��ǰ��������Ŀ�
		int sh = monitorInfo.WorkRect.Height;//��ǰ��������ĸ�

		this->PublicData.Scale = scanle;
		width = width * this->PublicData.Scale + 0.5;
		height = height * this->PublicData.Scale + 0.5;

		if (owner) {
			//���ڸ����ڵ����ĵ�
			RECT ownerRECT;
			::GetWindowRect(owner, &ownerRECT);
			int onwerWidth = ownerRECT.right - ownerRECT.left;
			int onwerHeight = ownerRECT.bottom - ownerRECT.top;
			if (width > 0 && height > 0) {
				_rect.X = ownerRECT.left + (onwerWidth - width) / 2.0f + 0.5;
				_rect.Y = ownerRECT.top + (onwerHeight - height) / 2.0f + 0.5;
			}
		}
		else {
			//������Ļ�����ĵ�
			_rect.X = x + (sw - width) / 2.0f + 0.5;//��֤���Ҿ���
			_rect.Y = y + (sh - height) / 2.0f + 0.5;//��֤���¾���
		}

		_rect.Width = width;
		_rect.Height = height;

		PublicData.HANDLE = ::CreateWindowExW(exStyle | WS_EX_ACCEPTFILES, Base::WindowClassName, Base::WindowClassName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dStyle,
			_rect.X, _rect.Y, width, height, owner, NULL, GetModuleHandle(NULL), NULL);

		//����ð����ʾ����
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
			::GetModuleHandle(NULL),
			NULL
		);


		PublicData.HANDLE = Hwnd();
		PublicData.Window = this;

		if ((exStyle & WS_EX_LAYERED) != WS_EX_LAYERED) {
			PublicData.InvalidateRect = [=](void* _rect)->void {
				RECT r;
				r.left = ((Rect*)_rect)->GetLeft();
				r.top = ((Rect*)_rect)->GetTop();
				r.right = ((Rect*)_rect)->GetRight();
				r.bottom = ((Rect*)_rect)->GetBottom();
				::InvalidateRect(Hwnd(), &r, FALSE);
				};
			PublicData.UpdateWindow = [=]()->void {
				::UpdateWindow(Hwnd());
				};
		}
		PublicData.SetTips = [=](Control* ctl, const std::wstring& text)->void {

			// ö�ٲ�ɾ��ÿ����ʾ��
			int toolCount = SendMessage(_hWndTips, TTM_GETTOOLCOUNT, 0, 0);
			for (int i = 0; i < toolCount; i++) {
				TOOLINFO toolInfo{ 0 };
				toolInfo.cbSize = sizeof(TOOLINFO);
				toolInfo.hwnd = Hwnd();
				toolInfo.uFlags = TTF_IDISHWND;
				toolInfo.hwnd = _hWndTips;
				// ���� TTM_ENUMTOOLS ��Ϣ�Ի�ȡ��ʾ����Ϣ
				if (SendMessage(_hWndTips, TTM_ENUMTOOLS, i, (LPARAM)&toolInfo)) {
					// ���� TTM_DELTOOL ��Ϣɾ����ʾ��
					SendMessage(_hWndTips, TTM_DELTOOL, 0, (LPARAM)&toolInfo);
				}
			}
			//EString str;
			//EString::UnicodeToANSI(text, &str);
			//printf("tips: %s\n", str.c_str());
			if (!text.empty()) {
				// ���� TTM_DELALLTOOL ��Ϣ
				TOOLINFO	tti{ 0 };
				tti.cbSize = sizeof(TOOLINFO);
				tti.uFlags = TTF_SUBCLASS;// | TTF_TRACK;
				tti.hwnd = Hwnd();
				tti.rect = ctl->ClipRect.ToRECT();
				tti.uId = (UINT_PTR)ctl;
				tti.lpszText = (LPWSTR)text.c_str();
				//���һ��tips��Ϣ
				SendMessage(_hWndTips, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
			}
			};
		PublicData.DelTips = [=](Control* ctl)->void {
			TOOLINFO	tti{ 0 };
			tti.cbSize = sizeof(TOOLINFO);
			tti.hwnd = Hwnd();
			tti.uId = (UINT_PTR)ctl;
			//�Ƴ�
			SendMessage(_hWndTips, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&tti);
			};
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
		UI_SET_USERDATA(Hwnd(), &PublicData);
	}

	Control* Window::FindControl(const EString& objectName)
	{
		if (!_layout) {
			return NULL;
		}
		return this->_layout->FindControl(objectName);
	}

	const HWND& Window::Hwnd()
	{
		return PublicData.HANDLE;
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
		::SetWindowTextW(Hwnd(), text.utf16().c_str());
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
		int newX = PublicData.Scale * rect.X + 0.5;
		int newY = PublicData.Scale * rect.Y + 0.5;
		int newWidth = PublicData.Scale * rect.Width + 0.5;
		int newHeight = PublicData.Scale * rect.Height + 0.5;
		::SetWindowPos(Hwnd(), NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	void Window::SetMiniSize(const Size& size)
	{
		_miniSize = size;
		_miniSize.Scale(this->PublicData.Scale);
	}
	void Window::SetMaxSize(const Size& size)
	{
		_maxSize = size;
		_maxSize.Scale(this->PublicData.Scale);
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
		_layout = layout;
		if (_layout->Style.FontFamily.empty()) {
			_layout->Style.FontFamily = L"Microsoft YaHei";
		}
		if (_layout->Style.FontSize == 0) {
			_layout->Style.FontSize = 12;
		}
		if (_layout->Style.ForeColor.GetValue() == 0) {
			_layout->Style.ForeColor = Color::Black;
		}
		_layout->PublicData = &PublicData;
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
	void Window::Show(int cmdShow)
	{
		ASSERT(_layout);
		::ShowWindow(Hwnd(), cmdShow);
		if (IsVisible()) {
			_layout->Refresh();
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
	int Window::ShowModal(bool disableOnwer)
	{
		//�˴����벻��������� ����رմ���ʱ,owner������˸����
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

	LRESULT  Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//if (uMsg == WM_QUIT) {
		//	int pause = 0;
		//}
		//if (WM_COMMAND == uMsg) {
		//	//windows�ӿؼ���Ϣ
		//}
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
			//�Ƿ�Ϊ����ʾ��
			if ((monitor.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY) {
				//��֤��������󻯵�ʱ��ʼ���ڹ����� �����ڵ�������
				RECT& rcWork = monitor.rcWork;
				pMMInfo->ptMaxPosition.x = rcWork.left;
				pMMInfo->ptMaxPosition.y = rcWork.top;
				pMMInfo->ptMaxSize.x = rcWork.right - rcWork.left;
				pMMInfo->ptMaxSize.y = rcWork.bottom - rcWork.top;
			}
			else {
				//��ȡ����ʾ��
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
					y -= 1;//����!�������λ�ú͵��ڴ��ڵĸ� ��ô���뷨�ͻ��ܵ����Ͻ�ȥ
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
			//�µ����ű�
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
			//	// �����������ʾ����
			//	pDispInfo->szText = const_cast<TCHAR*>(L"�µ���ʾ����");
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
			//��ȡ�ͻ����ľ���
			RECT rect;
			::GetClientRect(Hwnd(), &rect);
			Point clientPoint{ rect.left,rect.top };
			Size clientSize{ rect.right - rect.left,rect.bottom - rect.top };
			//�ͻ���������Ч��ʱ��
			if (clientSize.Width == 0 && clientSize.Height == 0) {
				//_rect = { 0,0,0,0 };
				return TRUE;
			}
			_rect = Rect{ wPos->x,wPos->y,wPos->cx,wPos->cy };
			_rectClient = Rect{ clientPoint,clientSize };
			//����
			OnRect(_rect);
			if (!_lastSize.Equals(clientSize)) {
				_lastSize = clientSize;
				OnSize(clientSize);
			}
			if (rePaint) {
				//Debug::Log("SWP_NOCOPYBITS!");
				//�������������������ݡ� ���δָ���˱�־������ڵ��������¶�λ���ں󱣴沢���ƻع���������Ч���ݡ�
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
				//����رմ���ʱ owner������˸������
				if (_oWnerWnd) {
					::EnableWindow(_oWnerWnd, TRUE);
				}
				::DestroyWindow(Hwnd());
			}
			else {
				//�ر���ȡ��
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
			this->PublicData.HANDLE = NULL;
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
				Control* outCtl = this->FindControl(point, &relativePoint);//�ҵ���ǰ�ؼ���λ��
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
			//��hwndTip������Ϣ���������ƶ���ʲôλ����
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
		//����ؼ��ǵ��ɵ������ֱ�Ӵ�͸
		if (dynamic_cast<Spacer*>(outCtl) && outCtl->Parent) {
			return  outCtl->Parent;
		}
		////�����̵��¼��ǿ��Դ�͸��(������ò�Ʋ��Ǻܺ�)
		//if ((outCtl->MousePassThrough & Event::OnMouseEvent || outCtl->MousePassThrough & Event::OnKeyBoardEvent) && outCtl->Parent) {
		//	return outCtl->Parent;
		//}
		return outCtl;
	}

	void Window::OnMouseMove(const Point& point)
	{
		if (_inputControl && _mouseDown) { //��ס�ƶ��Ŀؼ�
			auto ctlRect = _inputControl->GetClientRect();
			MouseEventArgs args(Event::OnMouseMove, { point.X - ctlRect.X ,point.Y - ctlRect.Y });
			_inputControl->DispatchEvent(args);
			return;
		}

		Point relativePoint;
		Control* newCtl = this->FindControl(point, &relativePoint);//�ҵ���ǰ�ؼ���λ��
		MouseEventArgs args(Event::None);
		args.Location = relativePoint;

		bool ok = true;
		//������һ��
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
		//�������е�
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
		//Ѱ��ؼ�
		Point relativePoint;
		Control* outCtl = this->FindControl(point, &relativePoint);
		//��������Ĳ�����һ�� ��ô��һ������ʧȥ�����¼�
		if (_inputControl != outCtl) {
			if (_inputControl) {
				KillFocusEventArgs arg(outCtl);
				_inputControl->DispatchEvent(arg);//����һ�����뽹�㴥��ʧȥ������¼�
			}
			_inputControl = outCtl;
		}
		//�����еĿؼ�������갴���¼�
		if (_inputControl) {
			MouseEventArgs args(Event::OnMouseDown);
			args.Button = mbtn;
			args.Location = relativePoint;
			_inputControl->DispatchEvent(args);
		}
		//��˫����Ϣ����
		auto _time = std::chrono::system_clock::now();
		auto diff = _time - _lastDownTime;
		auto timeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
		if (timeOffset < 300 && _lastBtn == mbtn) {//300����֮��ͬһ����ť����������˫����Ϣ
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
			//����̧���¼�
			{
				args.EventType = Event::OnMouseUp;
				_inputControl->DispatchEvent(args);

			}
			//���������¼� ������㻹�ڲ������δ�Ƴ��ؼ��� 
			if (_inputControl && mbtn == _lastBtn && ctlRect.Contains(point)) {
				args.EventType = Event::OnMouseClick;
				_inputControl->DispatchEvent(args);
			}
			if (_inputControl) {
				POINT p1{ 0 };
				::GetCursorPos(&p1);
				::ScreenToClient(Hwnd(), &p1);
				if (ctlRect.Contains(p1.x, p1.y) && ::GetForegroundWindow() == Hwnd()) {
					args.EventType = Event::OnMouseEnter;//������������¼�
				}
				else {
					args.EventType = Event::OnMouseLeave;//��������뿪�¼�
				}
				_inputControl->DispatchEvent(args);
			}
		}
	}

	void Window::OnMouseClick(MouseButton mbtn, const Point& point) {
	}

	void Window::OnSize(const Size& sz)
	{
		if (!_layout) {
			return;
		}
		if (_layout->GetScale() != PublicData.Scale) {
			this->OnDpiChange(PublicData.Scale, Rect());
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
	void Window::OnLocation(const Point& point) {
	}
	void Window::OnKillFocus(HWND hWnd)
	{
	}

	void Window::MoveWindow() {
		::ReleaseCapture();
		SendMessage(Hwnd(), WM_NCLBUTTONDOWN, HTCAPTION, NULL);//ģ����갴ס�������ƶ�����
		SendMessage(Hwnd(), WM_LBUTTONUP, NULL, NULL);//�ɿ�
	}

	void Window::OnDpiChange(const float& systemScale, const Rect& newRect)
	{
		//�µ����ű�
		float newScale = systemScale / PublicData.Scale;
		this->PublicData.Scale = systemScale;
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
			if (sender->Action == ControlAction::MoveWindow || sender == _layout) {
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
			if (sender->Action == ControlAction::MoveWindow || sender == _layout) {
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
			//�����ఴť����
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
						pos++;
					}
				}
			}

		}
		return false;
	}

};