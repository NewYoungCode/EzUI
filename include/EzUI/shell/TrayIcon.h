#pragma once
#include "Menu.h"
#include <shellapi.h>
namespace ezui {
	//系统托盘类
	class EZUI_API TrayIcon :public Object
	{
	private:
		WindowHandle m_hWnd = NULL;
		SystemMenu* m_menu = NULL;
		NOTIFYICONDATAW& m_nid;
		WindowBridge m_publicData;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		//事件处理 只会返回鼠标事件
		std::function<void(const MouseEventArgs&)> EventHandler = NULL;

		TrayIcon(Object* ownerObject = NULL);

		//设置图标
		void SetIcon(HICON icon);

		//设置鼠标悬停时显示的提示文本
		void SetTips(const UIString& text);

		//设置右键菜单
		void SetMenu(SystemMenu* menu);

		//提示消息
		void ShowMessage(const UIString& title, const UIString& msg, int timeOut = 1000);

		virtual ~TrayIcon();
	};

};