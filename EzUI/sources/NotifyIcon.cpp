#include "NotifyIcon.h"

namespace EzUI {
	bool __Init__RegeditClass__ = false;
	LRESULT CALLBACK _NotifyIcon_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR USERDATA = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		NotifyIcon* ntfi = (NotifyIcon*)USERDATA;

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
		::HINSTANCE hInstance = GetModuleHandleW(NULL);

		if (!__Init__RegeditClass__) {
			::WNDCLASSW    wc{ 0 };
			wc.lpfnWndProc = _NotifyIcon_WndProc;//窗口过程
			wc.hInstance = hInstance;//
			wc.hCursor = ::LoadCursorW(NULL, IDC_ARROW);//光标
			wc.lpszClassName = L"Class_EzUI_NotifyIcon";//类名
			RegisterClassW(&wc); //注册窗口
			__Init__RegeditClass__ = true;
		}
		_menu = NULL;
		_hInstance = hInstance;
		_hwnd = ::CreateWindowW(L"Class_EzUI_NotifyIcon", L"EzUI_NotifyIcon", WS_OVERLAPPEDWINDOW,
			0, 0, 10, 10, NULL, NULL, _hInstance, NULL);
		UI_SetUserData(_hwnd, this);
		_nid.cbSize = sizeof(_nid);//结构体长度
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
		_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
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