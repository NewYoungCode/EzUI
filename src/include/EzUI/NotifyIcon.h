#pragma once
#include "Menu.h"
namespace EzUI {
	extern bool __Init__RegeditClass__;
	class UI_EXPORT  NotifyIcon
	{
	public:
		HMODULE _hInstance;
		HWND _hwnd;
		std::function<bool(UINT)> _messageCallback = NULL;
		NOTIFYICONDATAW _nid;
		Menu* _menu;
		UINT _msgId = 0;
	public:
		//��Ҫ�Զ���һ��WIN32��Ϣ��������ʹ��
		NotifyIcon(UINT customMsg);
		void SetIcon(short id);
		void SetIcon(HICON icon);
		void SetText(const WCHAR* text);
		void SetMenu(Menu* menu);
		void ShowBalloonTip(const EString& title, const EString& msg, int timeOut = 1000);
		void SetMessageProc(const std::function<bool(UINT)>& messageCallback);
		virtual ~NotifyIcon();
	};

};