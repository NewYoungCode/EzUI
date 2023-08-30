#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
namespace EzUI {
	/// <summary>
	/// ����ʽ����(ʧȥ���㴰�ڽ���ر�) һ���������Ҽ��˵��ȵ�
	/// </summary>
	class  UI_EXPORT PopupWindow :public LayeredWindow {
	private:
		Control* _ownerCtl = NULL;
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
	public:
		PopupWindow(int width, int height, HWND ownerHwnd);
		PopupWindow(int width, int height, Control* ownerCtl = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW)override;
		virtual int ShowModal(bool disableOnwer = false)override;
	};

};