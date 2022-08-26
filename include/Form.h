#pragma once
#include "Window.h"
#include "BoxShadow.h"
namespace EzUI {

	//经典带边框WIN32窗口样式
	class UI_EXPORT FrameWindow :public Window
	{
	public:
		FrameWindow(int cx, int cy, HWND owner = NULL);
	};

#define SHADOWWIDTH 12
	//无边框带阴影的窗口 常规接受WM_PAINT消息
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = SHADOWWIDTH;
	protected:
		void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		BoxShadow* _boxShadow = NULL;
		void SetShadow(int width);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		void CloseShadow();
		void Hide();
	};
	//无边框 带阴影 分层窗口 不接受WM_PAINT消息 与常规窗口绘制消息不同 优点:屏幕外拖入窗体不会卡顿,不使用双缓冲 不闪烁 缺点:窗口大小发生改变时候会剧烈抖动
	class  UI_EXPORT LayeredWindow :public Window
	{
	private:
		int _shadowWidth = SHADOWWIDTH;
		void PushDC(HDC hdc);
	protected:
		EBitmap* _winBitmap = NULL;
		virtual void OnSize(const Size& sz)override;
		void OnPaint(HDC hdc, const Rect& rePaintRect) override;
		void OnRect(const Rect& rect);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		BoxShadow* _boxShadow = NULL;
		LayeredWindow(int cx, int cy, HWND owner = NULL);
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};

	class  UI_EXPORT MenuWindow :public BorderlessWindow {
	private:
	public:
		MenuWindow(int cx, int cy, HWND owner = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW);
	};
	using Form = BorderlessWindow;

};