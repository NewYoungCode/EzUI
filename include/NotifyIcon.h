#pragma once
#include "EzUI.h"
#include "Menu.h"
namespace EzUI {


	class UI_EXPORT  NotifyIcon
	{
	public:
		HMODULE _hInstance;
		HWND _hwnd;
		std::function<bool(UINT)> _messageCallback = NULL;
		NOTIFYICONDATAW _nid;
		Menu* _menu;
	public:
		NotifyIcon();
		void SetIcon(short id);
		void SetIcon(HICON icon);
		void SetText(const WCHAR* text);
		void SetMenu(Menu* menu);
		void ShowBalloonTip(const EString& title, const EString& msg, int timeOut = 1000);
		void SetMessageProc(const std::function<bool(UINT)>& messageCallback);
		virtual ~NotifyIcon();
	};

};