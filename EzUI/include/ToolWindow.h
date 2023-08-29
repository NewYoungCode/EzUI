#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
namespace EzUI {
	class  UI_EXPORT MenuWindow :public LayeredWindow {
	private:
		Control* _ownerCtl = NULL;
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
	public:
		MenuWindow(int width, int height, HWND ownerHwnd);
		MenuWindow(int width, int height, Control* ownerCtl = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW)override;
		virtual int ShowModal()override;
	};
	
};