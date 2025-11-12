#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"

namespace ezui {
	/// <summary>
	/// 弹出式窗口(弹出时会前置并抢占焦点 失去焦点窗口将会关闭或隐藏)
	/// </summary>
	class UI_EXPORT PopupWindow :public LayeredWindow {
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
	public:
		//弹出的窗口在拥有窗口前面 ownerHwnd为NULL则置顶窗口
		PopupWindow(int width = 0, int height = 0);
		virtual void Show()override;
		virtual ~PopupWindow();
	};
};