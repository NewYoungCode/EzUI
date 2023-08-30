#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
namespace EzUI {
	/// <summary>
	/// 弹出式窗口(失去焦点窗口将会关闭) 一般用于做右键菜单等等
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