#pragma once
#include "EzUI.h"
extern LRESULT CALLBACK Global_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
extern ULONG_PTR gdiplusToken;
class UI_EXPORT Application
{
public:
	Application();
	virtual ~Application();
	int exec();
	static void exit();
};

