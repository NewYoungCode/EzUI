#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
namespace EzUI {
	class  UI_EXPORT MenuWindow :public BorderlessWindow {
	public:
		MenuWindow(int cx, int cy, HWND owner = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW);
	};
	//Window            //经典带边框带系统菜单WIN32窗口样式
	//BorderlessWindow //无边框 带阴影 不透明窗口   屏幕外拖入会重绘     常规接受WM_PAINT消息 
	//LayeredWindow	  //无边框  带阴影 窗口透明异形 窗口大小发生改变重绘 自定义UI_PAINT消息(定时检测无效区域) 不允许有子窗口 
	using Form = BorderlessWindow;
};