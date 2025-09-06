#include "NotifyIcon.h"
#define TRAY_ICON_MSG WM_APP+1 // 自定义托盘消息

namespace ezui {
	bool g_bClassRegistered = false;
	NotifyIcon::NotifyIcon(Object* parentObj) :Object(parentObj)
	{
		if (!g_bClassRegistered) {
			::WNDCLASSEXW wcex = {};
			wcex.cbSize = sizeof(wcex);
			wcex.lpfnWndProc = [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT {
				WindowContext* wndData = (WindowContext*)UI_GET_USERDATA(hwnd);
				if (wndData && wndData->WndProc) {
					return wndData->WndProc(hwnd, message, wParam, lParam);
				}
				return ::DefWindowProc(hwnd, message, wParam, lParam);
				};
			wcex.hInstance = ezui::__EzUI__HINSTANCE;
			wcex.lpszClassName = L"EzUI_NotifyIcon";
			RegisterClassExW(&wcex);
			g_bClassRegistered = true;
		}
		m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, L"EzUI_NotifyIcon", L"", 0, 0, 0, 0, 0, NULL, NULL, ezui::__EzUI__HINSTANCE, NULL);
		m_publicData.WndProc = [this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT {
			return this->WndProc(uMsg, wParam, lParam);
			};
		UI_SET_USERDATA(m_hWnd, (LONG_PTR)&m_publicData);
		m_nid.cbSize = sizeof(m_nid);//结构体长度
		m_nid.hWnd = m_hWnd;//窗口句柄
		m_nid.uCallbackMessage = TRAY_ICON_MSG;//消息处理,这里很重要,处理鼠标点击
		m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIconW(NIM_ADD, &m_nid);
	}

	LRESULT NotifyIcon::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == TRAY_ICON_MSG) {
			POINT point;
			GetCursorPos(&point);
			switch (LOWORD(lParam))
			{
			case WM_MOUSEMOVE: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseMove, { point.x, point.y }, MouseButton::None);
					EventHandler(args);
				}
				break;
			}
			case WM_LBUTTONDOWN: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseDown, { point.x, point.y }, MouseButton::Left);
					EventHandler(args);
				}
				break;
			}
			case WM_LBUTTONUP: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseUp, { point.x, point.y }, MouseButton::Left);
					EventHandler(args);
				}
				break;
			}
			case WM_LBUTTONDBLCLK: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseDoubleClick, { point.x, point.y }, MouseButton::Left);
					EventHandler(args);
				}
				break;
			}
			case WM_RBUTTONDOWN: {
				if (this->EventHandler) {
					MouseEventArgs args(Event::OnMouseDown, { point.x,point.y }, MouseButton::Right);
					this->EventHandler(args);
				}
				if (m_menu) {
					//如果设置了托盘菜单则弹出菜单
					SetForegroundWindow(m_hWnd);
					TrackPopupMenu(m_menu->HMenu(), TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
				}
				break;
			}
			case WM_RBUTTONUP: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseUp, { point.x, point.y }, MouseButton::Right);
					EventHandler(args);
				}
				break;
			}
			case WM_RBUTTONDBLCLK: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseDoubleClick, { point.x, point.y }, MouseButton::Right);
					EventHandler(args);
				}
				break;
			}
			case WM_MBUTTONDOWN: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseDown, { point.x, point.y }, MouseButton::Middle);
					EventHandler(args);
				}
				break;
			}
			case WM_MBUTTONUP: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseUp, { point.x, point.y }, MouseButton::Middle);
					EventHandler(args);
				}
				break;
			}
			case WM_MBUTTONDBLCLK: {
				if (EventHandler) {
					MouseEventArgs args(Event::OnMouseDoubleClick, { point.x, point.y }, MouseButton::Middle);
					EventHandler(args);
				}
				break;
			}
			default:
				break;
			}
		}
		else if (uMsg == WM_COMMAND) {
			//菜单被点击
			auto id = LOWORD(wParam);
			if (m_menu && m_menu->MouseClick) {
				m_menu->MouseClick(id);
			}
		}
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
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

	void NotifyIcon::SetTips(const UIString& text)
	{
		wcscpy_s(m_nid.szTip, text.unicode().c_str());
		Shell_NotifyIconW(NIM_MODIFY, &m_nid);
	}

	void NotifyIcon::ShowBalloonTip(const UIString& title, const UIString& msg, int timeOut) {
		m_nid.uTimeout = timeOut;
		m_nid.uFlags = NIF_INFO;
		m_nid.dwInfoFlags = NIIF_INFO;
		wcscpy_s(m_nid.szInfoTitle, title.unicode().c_str());
		wcscpy_s(m_nid.szInfo, msg.unicode().c_str());
		Shell_NotifyIconW(NIM_MODIFY, &m_nid);
	}

	NotifyIcon::~NotifyIcon()
	{
		if (::IsWindow(m_hWnd)) {
			Shell_NotifyIconW(NIM_DELETE, &m_nid);
			::DestroyWindow(m_hWnd);
		}
	}
};