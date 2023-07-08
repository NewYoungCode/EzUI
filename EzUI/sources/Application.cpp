#include "Application.h"

namespace EzUI {

	LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		WindowData* wndData = (WindowData*)UI_GetUserData(hwnd);
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	void Application::Init() {
		//初始化公共控件库
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_WIN95_CLASSES;  // 或者使用其他需要的控件类别
		InitCommonControlsEx(&icex);
		//禁用DPI感知 避免应用程序自动放大
		//SetProcessDPIAware();
		//设计窗口
		::HINSTANCE hInstance = GetModuleHandleW(NULL);
		::WNDCLASSW    wc{ 0 };
		wc.lpfnWndProc = EzUI_WndProc;//窗口过程
		wc.hInstance = hInstance;//
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);//光标
		wc.lpszClassName = WindowClassName;//类名

		if (!RegisterClassW(&wc)) //注册窗口
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		::SetCurrentDirectoryW(Application::StartPath().c_str());
		::CoInitialize(NULL);//初始化com
		RenderInitialize();
	}
	Application::Application(int resID, const EString& custResType, const EString& password) {
		Init();
		HINSTANCE hInst = ::GetModuleHandle(NULL);
#ifdef UNICODE
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(resID), custResType.utf16().c_str());
#else
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(resID), custResType.c_str());
#endif
		if (!hRsrc)return;
		DWORD len = SizeofResource(hInst, hRsrc);
		HVSResource = LoadResource(hInst, hRsrc);
		HZipResource = OpenZip((void*)HVSResource, len, password.empty() ? NULL : password.c_str());
	}
	//使用本地文件名称加载资源包
	Application::Application(const EString& fileName, const EString& password) {
		Init();
		HZipResource = OpenZip(fileName.utf16().c_str(), password.empty() ? NULL : password.c_str());
#ifdef _DEBUG
		if (HZipResource == NULL) {
			::MessageBoxW(NULL, fileName.utf16().c_str(), L"Failed to open zip", MB_ICONWARNING);
		}
#endif
	}
	Application::Application() {
		Init();
	}
	Application::~Application() {
		RenderUnInitialize();
		::CoUninitialize();
		if (HZipResource) {
			CloseZip(HZipResource);
		}
		if (HVSResource) {
			FreeResource(HVSResource);
		}
		UnregisterClassW(WindowClassName, GetModuleHandle(NULL));
	}

	int Application::exec()
	{
		::MSG msg{ 0 };
		while (GetMessageW(&msg, NULL, 0, 0) != 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
		return (int)msg.wParam;
	}

};