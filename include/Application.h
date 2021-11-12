#pragma once
#include "EzUI.h"
extern LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
class UI_EXPORT Application
{
public:
	Application();
	virtual ~Application();
	int exec();
	static void exit(int exitCode = 0) {
		PostQuitMessage(exitCode);
	}
};

