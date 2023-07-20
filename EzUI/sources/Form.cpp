#include "Form.h"

namespace EzUI {
	MenuWindow::MenuWindow(int width, int height, HWND owner) :BorderlessWindow(width, height, owner)
	{
		this->Zoom = false;
	}
	LRESULT MenuWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KILLFOCUS) {
			HWND wnd = (HWND)wParam;
			if (wnd != this->_boxShadow->_hWnd) {
				this->OnKillFocus(wnd);
			}
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void MenuWindow::Show(int cmdShow)
	{
		auto rect = this->GetClientRect();
		//��ȡ��ʾ�� �ﵽ������ĸ���ʾ�� ���ھ����ĸ���ʾ������ʾ
		std::list<MonitorInfo> outMonitorInfo;
		GetMonitors(&outMonitorInfo);
		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		MonitorInfo* monitorInfo = NULL;
		for (auto& it : outMonitorInfo) {
			if (it.Rect.Contains(cursorPos.x, cursorPos.y)) {
				monitorInfo = &it;
				break;
			}
		}
		int x = cursorPos.x;
		int y = cursorPos.y;
		int width = rect.Width;
		int height = rect.Height;
		if ((cursorPos.y + height) > monitorInfo->Rect.Height) {
			y -= height;
		}
		if ((cursorPos.x + width) > monitorInfo->Rect.Width) {
			x -= width;
		}
		__super::Show(cmdShow);
		::SetForegroundWindow(Hwnd());
		::SetWindowPos(Hwnd(), HWND_TOPMOST, x, y, width, height, NULL);
	}
	void MenuWindow::OnKillFocus(HWND hWnd)
	{
		this->Close();
	}
};