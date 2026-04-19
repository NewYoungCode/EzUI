#pragma once
#include "BorderlessWindow.h"

namespace ezui {
	/// <summary>
	/// 弹出式窗口(弹出时会前置并抢占焦点 失去焦点窗口将会关闭或隐藏)
	/// </summary>
	class EZUI_API PopupWindow :public BorderlessWindow {
	protected:
		virtual void OnKillFocus(WindowHandle hWnd) override;
	public:
		//弹出的窗口在拥有窗口前面 ownerHwnd为NULL则置顶窗口
		explicit PopupWindow(WindowHandle owner = NULL);
		explicit PopupWindow(Window* ownerWnd);
		virtual void Show()override;
		virtual ~PopupWindow();
	};
};