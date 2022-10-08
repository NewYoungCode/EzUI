#pragma once
#include "Window.h"
#include "ShadowWindow.h"
namespace EzUI {
	//无边框 带阴影 分层窗口 不接受WM_PAINT消息 与常规窗口绘制消息不同 优点:屏幕外拖入窗体不会卡顿,不使用双缓冲 不闪烁 
	class  UI_EXPORT LayeredWindow :public Window
	{
	private:
		int _shadowWidth = SHADOWWIDTH;
		void PushDC(HDC hdc);
		Rect _InvalidateRect;
		bool bRunTask = true;
		std::thread* task = NULL;
	protected:
		void InvalidateRect(const Rect& rect);
		EBitmap* _winBitmap = NULL;
		virtual void OnSize(const Size& sz)override;
		void OnPaint(HDC hdc, const Rect& rePaintRect) override;
		void OnRect(const Rect& rect);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		ShadowWindow* _boxShadow = NULL;
		LayeredWindow(int cx, int cy, HWND owner = NULL);
		void UpdateShadow();
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};
};
