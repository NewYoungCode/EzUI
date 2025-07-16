#include "NotifyIcon.h"

namespace ezui {
	bool __Init__RegeditClass__ = false;
	LRESULT CALLBACK __NotifyIcon_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR USERDATA = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		NotifyIcon* ntfi = (NotifyIcon*)USERDATA;
		do
		{
			if (ntfi == NULL)break;
			if (message == WM_USER+1) {
				if (ntfi->MessageCallback) {
					ntfi->MessageCallback(lParam);
				}
				if (lParam == WM_RBUTTONDOWN && ntfi->m_menu && ntfi->m_menu->m_hMenu) {
					POINT           point;
					GetCursorPos(&point);
					SetForegroundWindow(hwnd);
					TrackPopupMenu(ntfi->m_menu->m_hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
				}
				break;
			}
			if (message == WM_COMMAND) {
				auto id = LOWORD(wParam);
				if (ntfi->m_menu && ntfi->m_menu->Callback) {
					ntfi->m_menu->Callback(id);
				}
				break;
			}
		} while (false);
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	NotifyIcon::NotifyIcon()
	{
		::HINSTANCE hInstance = ezui::__EzUI__HINSTANCE;

		if (!__Init__RegeditClass__) {
			::WNDCLASSW    wc{ 0 };
			wc.lpfnWndProc = __NotifyIcon_WndProc;//窗口过程
			wc.hInstance = hInstance;//
			wc.hCursor = ::LoadCursorW(NULL, IDC_ARROW);//光标
			wc.lpszClassName = L"EzUI_NotifyIcon";//类名
			RegisterClassW(&wc); //注册窗口
			__Init__RegeditClass__ = true;
		}
		m_menu = NULL;
		m_hInstance = hInstance;
		m_hwnd = ::CreateWindowW(L"EzUI_NotifyIcon", L"EzUI_NotifyIcon", WS_OVERLAPPEDWINDOW,
			0, 0, 10, 10, NULL, NULL, m_hInstance, NULL);
		UI_SET_USERDATA(m_hwnd, this);
		m_nid.cbSize = sizeof(m_nid);//结构体长度
		m_nid.hWnd = m_hwnd;//窗口句柄
		m_nid.uCallbackMessage = WM_USER + 1;//消息处理，这里很重要，处理鼠标点击
		m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIconW(NIM_ADD, &m_nid);
	}

	void NotifyIcon::SetIcon(short id)
	{
		SetIcon(::LoadIcon(m_hInstance, MAKEINTRESOURCE(id)));//加载图标
	}

	void NotifyIcon::SetIcon(HICON icon)
	{
		m_nid.hIcon = icon;
		m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIconW(NIM_MODIFY, &m_nid);
	}

	void NotifyIcon::SetMenu(Menu* menu) {
		this->m_menu = menu;
	}

	void NotifyIcon::SetText(const UIString& text)
	{
		wcscpy_s(m_nid.szTip, text.unicode().c_str());
		Shell_NotifyIconW(NIM_MODIFY, &m_nid);
	}

	void NotifyIcon::ShowBalloonTip(const UIString& title, const UIString& msg, int_t timeOut) {
		m_nid.uTimeout = timeOut;
		m_nid.uFlags = NIF_INFO;
		m_nid.dwInfoFlags = NIIF_INFO;
		wcscpy_s(m_nid.szInfoTitle, title.unicode().c_str());
		wcscpy_s(m_nid.szInfo, msg.unicode().c_str());
		Shell_NotifyIconW(NIM_MODIFY, &m_nid);
	}

	NotifyIcon::~NotifyIcon()
	{
		if (::IsWindow(m_hwnd)) {
			::DestroyWindow(m_hwnd);
		}
	}
};