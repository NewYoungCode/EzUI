#include "NotifyIcon.h"
bool _DuiLib_NotifyIcon_Class = false;

#pragma warning(disable:4996)
LRESULT CALLBACK _NotifyIcon_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR USERDATA = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	NotifyIcon *ntfi = dynamic_cast<NotifyIcon*>((NotifyIcon*)USERDATA);

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
	case WM_NOTIFYICON:
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
	if (!_DuiLib_NotifyIcon_Class) {
		//��ƴ���
		WNDCLASS     wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = _NotifyIcon_WndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = _hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = UI_NotifyIcon_CLASSNAME;
		RegisterClass(&wc);
		_DuiLib_NotifyIcon_Class = true;
	}

	_hwnd = ::CreateWindow(UI_NotifyIcon_CLASSNAME, UI_NotifyIcon_CLASSNAME, WS_OVERLAPPEDWINDOW,
		0, 0, 10, 10, NULL, NULL, _hInstance, NULL);
	::SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	_nid.cbSize = sizeof(NOTIFYICONDATA);//�ṹ�峤��
	_nid.hWnd = _hwnd;//���ھ��
	_nid.uCallbackMessage = WM_NOTIFYICON;//��Ϣ�����������Ҫ�����������
	_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIconW(NIM_ADD, &_nid);

}

void NotifyIcon::SetIcon(short id)
{
	SetIcon(::LoadIcon(_hInstance, MAKEINTRESOURCE(id)));//����ͼ��
}

void NotifyIcon::SetIcon(HICON icon)
{
	_nid.hIcon = icon;
	Shell_NotifyIconW(NIM_MODIFY, &_nid);

}

void NotifyIcon::SetMenu(Menu* menu) {

	this->_menu = menu;
}

void NotifyIcon::SetText(const WCHAR * text)
{
	wcscpy(_nid.szTip, text);
	Shell_NotifyIconW(NIM_MODIFY, &_nid);
}

void NotifyIcon::ShowBalloonTip(const EString&title, const EString&msg, int timeOut) {
	_nid.uTimeout = timeOut;
	_nid.uFlags = NIF_INFO;
	_nid.dwInfoFlags = NIIF_INFO;
	wcscpy(_nid.szInfoTitle, title.c_str());
	wcscpy(_nid.szInfo, msg.c_str());
	Shell_NotifyIconW(NIM_MODIFY, &_nid);
}

void NotifyIcon::SetMessageProc(const std::function<bool(UINT)> &messageCallback) {
	_messageCallback = messageCallback;
}

NotifyIcon::~NotifyIcon()
{
	if (::IsWindow(_hwnd)) {
		::SendMessage(_hwnd, WM_DESTROY, NULL, NULL);
	}
}
