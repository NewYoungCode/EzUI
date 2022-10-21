#include "Application.h"

namespace EzUI {

	LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		WindowData* wndData = (WindowData*)UI_GetUserData(hwnd);
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	Application::Application() {
		//确保加载公共控件 DLL （Comctl32.dll），并从 DLL 注册特定的公共控件类
		INITCOMMONCONTROLSEX iccex;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&iccex);

		//设计窗口
		::HINSTANCE hInstance = GetModuleHandle(NULL);
		::WNDCLASS     wc;
		wc.style = NULL;//CS_DBLCLKS;//放弃windows消息中的双击消息
		//wc.style = CS_HREDRAW | CS_VREDRAW;//| CS_DBLCLKS;
		wc.lpfnWndProc = EzUI_WndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		StdString className = GetThisClassName();
		wc.lpszClassName = className.c_str();
		if (!RegisterClass(&wc)) //注册窗口
		{
			::MessageBox(NULL, TEXT("This program requires Windows NT !"),
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		::SetCurrentDirectoryW(Application::StartPath().c_str());
		::CoInitialize(NULL);//初始化com
		RenderInitialize();
	}

	Application::~Application() {
		RenderUnInitialize();
		::CoUninitialize();
	}

	int Application::exec()
	{
		::MSG msg{ 0 };
		while (GetMessage(&msg, NULL, 0, 0) != 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}

};