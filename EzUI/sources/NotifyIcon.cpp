#include "NotifyIcon.h"

namespace EzUI {
	LRESULT CALLBACK _NotifyIcon_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR USERDATA = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		NotifyIcon* ntfi = dynamic_cast<NotifyIcon*>((NotifyIcon*)USERDATA);

		switch (message)
		{

		case WM_COMMAND:
		{
			short id = LOWORD(wParam);
			if (ntfi->_menu && ntfi->_menu->_callback) {
				ntfi->_menu->_callback(id);
			}
			break;
		}
		case  UI_NOTIFYICON:
		{
			if (ntfi->_messageCallback) {
				if (ntfi->_messageCallback(lParam)) {
					break;
				}
			}
			if (lParam == WM_RBUTTONDOWN && ntfi->_menu && ntfi->_menu->_hMenu) {
				POINT           point;
				GetCursorPos(&point);
				SetForegroundWindow(hwnd);
				TrackPopupMenu(ntfi->_menu->_hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
			}
			break;
		}
		case WM_DESTROY:
		{
			Shell_NotifyIconW(NIM_DELETE, &(ntfi->_nid));
			break;
		}

		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	NotifyIcon::NotifyIcon()
	{
		_menu = NULL;
		_hInstance = GetModuleHandle(0);
	
		_hwnd = ::CreateWindow(GetThisClassName().c_str(), TEXT("EzUI_NotifyIcon"), WS_OVERLAPPEDWINDOW,
			0, 0, 10, 10, NULL, NULL, _hInstance, NULL);
		::SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		_nid.cbSize = sizeof(NOTIFYICONDATA);//结构体长度
		_nid.hWnd = _hwnd;//窗口句柄
		_nid.uCallbackMessage = UI_NOTIFYICON;//消息处理，这里很重要，处理鼠标点击
		_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIconW(NIM_ADD, &_nid);

	}

	void NotifyIcon::SetIcon(short id)
	{
		SetIcon(::LoadIcon(_hInstance, MAKEINTRESOURCE(id)));//加载图标
	}

	void NotifyIcon::SetIcon(HICON icon)
	{
		_nid.hIcon = icon;
		Shell_NotifyIconW(NIM_MODIFY, &_nid);

	}

	void NotifyIcon::SetMenu(Menu* menu) {

		this->_menu = menu;
	}

	void NotifyIcon::SetText(const WCHAR* text)
	{
		wcscpy_s(_nid.szTip, text);
		Shell_NotifyIconW(NIM_MODIFY, &_nid);
	}

	void NotifyIcon::ShowBalloonTip(const EString& title, const EString& msg, int timeOut) {
		_nid.uTimeout = timeOut;
		_nid.uFlags = NIF_INFO;
		_nid.dwInfoFlags = NIIF_INFO;
		wcscpy_s(_nid.szInfoTitle, title.utf16().c_str());
		wcscpy_s(_nid.szInfo, msg.utf16().c_str());
		Shell_NotifyIconW(NIM_MODIFY, &_nid);
	}

	void NotifyIcon::SetMessageProc(const std::function<bool(UINT)>& messageCallback) {
		_messageCallback = messageCallback;
	}

	NotifyIcon::~NotifyIcon()
	{
		if (::IsWindow(_hwnd)) {
			::SendMessage(_hwnd, WM_DESTROY, NULL, NULL);
		}
	}
};