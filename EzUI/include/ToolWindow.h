#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
namespace EzUI {
	class  UI_EXPORT MenuWindow :public BorderlessWindow {
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
		Control* _ownerCtl = NULL;
	public:
		MenuWindow(int width, int height, HWND ownerHwnd);
		MenuWindow(int width, int height, Control* ownerCtl = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW)override;
		virtual int ShowModal()override;
	};
	
};