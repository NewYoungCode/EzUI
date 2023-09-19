#include "Application.h"

namespace EzUI {

	LRESULT CALLBACK EzUI_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		WindowData* wndData = (WindowData*)UI_GET_USERDATA(hWnd);
		//if (message == WM_CREATE)
		//{
		//	RECT rcClient;
		//	GetWindowRect(hWnd, &rcClient);
		//	// Inform the application of the frame change.
		//	SetWindowPos(hWnd,
		//		NULL,
		//		rcClient.left, rcClient.top,
		//		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		//		SWP_FRAMECHANGED);
		//}
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}

	void Application::Init() {
		//设计窗口
		::HINSTANCE hInstance = GetModuleHandleW(NULL);
		::WNDCLASSW    wc{ 0 };
		wc.lpfnWndProc = EzUI_WndProc;//窗口过程
		wc.hInstance = hInstance;//
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);//光标
		wc.lpszClassName = Base::WindowClassName;//类名
		if (!RegisterClassW(&wc)) //注册窗口
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		//初始化公共控件库
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_WIN95_CLASSES;  // 或者使用其他需要的控件类别
		::InitCommonControlsEx(&icex);
		//设置路径
		wchar_t wPath[MAX_PATH]{ 0 };
		size_t count = ::GetModuleFileNameW(NULL, wPath, MAX_PATH);
		for (int i = count - 1; i > -1; i--)
		{
			if (wPath[i] == L'\\') {
				wPath[i] = 0;
				break;
			}
		}
		::SetCurrentDirectoryW(wPath);
		::CoInitialize(NULL);//初始化com
		RenderInitialize();
	}
	void Application::EnableHighDpi() {
		EzUI::GetMonitors((std::list<MonitorInfo>*) & EzUI::Base::MonitorInfos);
		//DPI感知相关
		//不跟随系统放大无法接收WM_DISPLAYCHANGED消息
		//bool b = SetProcessDPIAware();
		//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		//会进入WM_DPICHANGED消息可进行自行控制缩放
		typedef void (WINAPI* DisableAutoDpi)(DWORD);
		HMODULE hModNtdll = NULL;
		if (hModNtdll = ::LoadLibraryW(L"User32.dll")) {
			DisableAutoDpi pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (DisableAutoDpi)::GetProcAddress(hModNtdll, "SetProcessDpiAwarenessContext");
			if (pfRtlGetNtVersionNumbers)
			{
				//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
				pfRtlGetNtVersionNumbers(-4);
			}
			else {
				::SetProcessDPIAware();
			}
			::FreeLibrary(hModNtdll);
		}
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
		Base::HVSResource = LoadResource(hInst, hRsrc);
		Base::HZipResource = OpenZip((void*)Base::HVSResource, len, password.empty() ? NULL : password.c_str());
	}
	//使用本地文件名称加载资源包
	Application::Application(const EString& fileName, const EString& password) {
		Init();
		Base::HZipResource = OpenZip(fileName.utf16().c_str(), password.empty() ? NULL : password.c_str());
#ifdef _DEBUG
		if (Base::HZipResource == NULL) {
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
		if (Base::HZipResource) {
			CloseZip(Base::HZipResource);
		}
		if (Base::HVSResource) {
			FreeResource(Base::HVSResource);
		}
		UnregisterClassW(Base::WindowClassName, GetModuleHandle(NULL));
	}

	int Application::Exec()
	{
		BOOL bRet;
		::MSG msg{ 0 };
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (bRet == -1)
			{
				// handle the error and possibly exit
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return (int)msg.wParam;
	}

};