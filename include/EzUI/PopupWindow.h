#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"

namespace ezui {
	/// <summary>
	/// 弹出式窗口(失去焦点窗口将会关闭) 一般用于做右键菜单等等
	/// </summary>
	class UI_EXPORT PopupWindow :public LayeredWindow {
	private:
		bool isShowModal = false;
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
	public:
		//弹出的窗口在拥有窗口前面 ownerHwnd为NULL则置顶窗口
		PopupWindow(int width, int height, HWND ownerHwnd = NULL);
		virtual void Show()override;
		virtual int ShowModal(bool disableOnwer = false)override;
		virtual ~PopupWindow();
	};
};