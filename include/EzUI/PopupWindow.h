#pragma once
#include "Window.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"

namespace ezui {
	/// <summary>
	/// 弹出式窗口(失去焦点窗口将会关闭) 一般用于做右键菜单等等
	/// </summary>
	class UI_EXPORT PopupWindow :public LayeredWindow {
	public:
		/*	enum class Model :int
			{
				None = 0,
				AutoWidth = 1,
				AutoHeight = 2,
				Top = 4,
				Bottom = 8
			};*/
	private:
		Control* m_ownerCtl = NULL;
	protected:
		virtual void OnKillFocus(HWND hWnd) override;
	public:
		PopupWindow(int_t width, int_t height, HWND ownerHwnd);
		PopupWindow(int_t width, int_t height, Control* ownerCtl = NULL);
		virtual void Show()override;
		virtual int_t ShowModal(bool disableOnwer = false)override;
		virtual ~PopupWindow();
	};
};