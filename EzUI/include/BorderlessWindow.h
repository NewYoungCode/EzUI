#pragma once
#include "Window.h"
#include "ShadowWindow.h"
namespace EzUI {
	//无边框带阴影的窗口 常规接受WM_PAINT消息
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = SHADOWWIDTH;
	protected:
		void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		ShadowWindow* _boxShadow = NULL;
		void SetShadow(int width);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		void UpdateShadow();
		void CloseShadow();
		void Hide();
	};
};
