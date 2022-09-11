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
	//Window            //������߿�WIN32������ʽ
	//BorderlessWindow //�ޱ߿� ����Ӱ ��͸������ �������WM_PAINT��Ϣ
	//LayeredWindow	  //�ޱ߿�  ����Ӱ ���ڿ�͸�� �Զ���UI_PAINT��Ϣ
	using Form = LayeredWindow;
};