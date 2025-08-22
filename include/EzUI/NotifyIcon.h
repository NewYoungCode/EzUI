#pragma once
#include "Menu.h"
#include <shellapi.h>
namespace ezui {
	class UI_EXPORT  NotifyIcon
	{
	public:
		HMODULE m_hInstance;
		HWND m_hwnd;
		NOTIFYICONDATAW m_nid;
		Menu* m_menu;
		std::function<bool(UINT)> MessageCallback = NULL;
	public:
		//需要自定义一个WIN32消息来供此类使用
		NotifyIcon();
		void SetIcon(HICON icon);
		void SetText(const UIString& text);
		void SetMenu(Menu* menu);
		void ShowBalloonTip(const UIString& title, const UIString& msg, int timeOut = 1000);
		virtual ~NotifyIcon();
	};

};