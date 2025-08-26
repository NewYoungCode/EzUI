#pragma once
#include "Menu.h"
#include <shellapi.h>
namespace ezui {
	//系统托盘类
	class UI_EXPORT NotifyIcon :public Object
	{
	private:
		HWND m_hWnd = NULL;
		Menu* m_menu = NULL;
		NOTIFYICONDATAW m_nid = {};
		WindowData m_publicData;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		//事件处理 只会返回鼠标事件
		std::function<void(const MouseEventArgs&)> EventHandler = NULL;
		NotifyIcon(Object* parentObj = NULL);
		void SetIcon(HICON icon);
		//设置鼠标悬停时显示的提示文本
		void SetTips(const UIString& text);
		void SetMenu(Menu* menu);
		void ShowBalloonTip(const UIString& title, const UIString& msg, int timeOut = 1000);
		virtual ~NotifyIcon();
	};

};