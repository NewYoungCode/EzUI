#pragma once
#include "Window.h"
#include "ShadowWindow.h"
namespace EzUI {
	/// <summary>
	/// BorderlessWindow //无边框 带阴影 不透明窗口   屏幕外拖入会重绘     常规接受WM_PAINT消息 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = 12;
		ShadowWindow* _boxShadow = NULL;
	protected:
		virtual void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		void UpdateShadow();
	public:
		void SetShadow(int padding);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		ShadowWindow* GetShadowWindow();
		void CloseShadow();
		void Hide();
	};
};
