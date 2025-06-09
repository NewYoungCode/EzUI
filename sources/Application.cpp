#include "Application.h"
#include "LayeredWindow.h"
#include <functional>
namespace EzUI {
	LRESULT CALLBACK __EzUI__WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
		WindowData* wndData = (WindowData*)UI_GET_USERDATA(hWnd);
		if (message == WM_CREATE) {
			__EzUI__WNDS.push_back(hWnd);
		}
		else if (message == WM_DESTROY) {
			auto itor = std::find(__EzUI__WNDS.begin(), __EzUI__WNDS.end(), hWnd);
			if (itor != __EzUI__WNDS.end()) {
				__EzUI__WNDS.erase(itor);
			}
		}
		else if (message == WM_GUI_SYSTEM) {
			if (wParam == WM_GUI_BEGININVOKE || wParam == WM_GUI_INVOKE) {
				using Func = std::function<void()>;
				Func* callback = (Func*)lParam;
				(*callback)();
				if (wParam == WM_GUI_BEGININVOKE) {
					delete callback;
				}
			}
			return 0;
		}
		else if (message == WM_GUI_APP) {
			return 0;
		}
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}

	void Application::Init() {
		//存入全局实例
		EzUI::__EzUI__HINSTANCE = ::GetModuleHandleW(NULL);
		EzUI::__EzUI__ThreadId = ::GetCurrentThreadId();
		//设计窗口
		::WNDCLASSW    wc{ 0 };
		wc.lpfnWndProc = __EzUI__WndProc;//窗口过程
		wc.hInstance = EzUI::__EzUI__HINSTANCE;//
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);//光标
		wc.lpszClassName = EzUI::__EzUI__WindowClassName;//类名
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
		DWORD count = ::GetModuleFileNameW(NULL, wPath, MAX_PATH);
		for (int_t i = count - 1; i > -1; i--)
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
		EzUI::GetMonitor((std::list<MonitorInfo>*) & EzUI::__EzUI__MonitorInfos);
		//DPI感知相关
		//不跟随系统放大无法接收WM_DISPLAYCHANGED消息
		//bool b = SetProcessDPIAware();
		//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		//会进入WM_DPICHANGED消息可进行自行控制缩放
		HMODULE hModNtdll = NULL;
		if (hModNtdll = ::LoadLibraryW(L"User32.dll")) {
			typedef void (WINAPI* Func_SetProcessDpiAwarenessContext)(void*);
			Func_SetProcessDpiAwarenessContext  setProcessDpiAwarenessContext;
			setProcessDpiAwarenessContext = (Func_SetProcessDpiAwarenessContext)::GetProcAddress(hModNtdll, "SetProcessDpiAwarenessContext");
			if (setProcessDpiAwarenessContext)
			{
				//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);//高版本sdk直接用
				setProcessDpiAwarenessContext((void*)-4);//函数指针方式是为了兼容win7
			}
			else {
				::SetProcessDPIAware();
			}
			::FreeLibrary(hModNtdll);
		}
	}

	bool Application::SetResource(const EString& localOrResName) {
		//只允许有一个资源文件
		if (EzUI::__EzUI__Resource) {
			delete EzUI::__EzUI__Resource;
			EzUI::__EzUI__Resource = NULL;
		}
		//先从vs中的资源里面查找
		bool found = false;
		HRSRC hRsrc = Resource::FindRC(localOrResName);
		if (hRsrc) {
			EzUI::__EzUI__Resource = new Resource(hRsrc);
			found = true;
		}
		else {
			//本地文件中获取
			std::wstring wstr = localOrResName.unicode();
			DWORD dwAttr = GetFileAttributesW(wstr.c_str());
			if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
				EzUI::__EzUI__Resource = new Resource(localOrResName);
				found = true;
			}
		}
#ifdef _DEBUG
		if (!found) {
			::MessageBoxW(NULL, localOrResName.unicode().c_str(), L"Failed to open Resource", MB_OK | MB_ICONINFORMATION);
		}
#endif
		return found;
	}
	Application::Application() {
		Init();
	}
	Application::~Application() {
		RenderUnInitialize();
		::CoUninitialize();
		if (EzUI::__EzUI__Resource) {
			delete EzUI::__EzUI__Resource;
		}
		UnregisterClassW(EzUI::__EzUI__WindowClassName, EzUI::__EzUI__HINSTANCE);
	}

	int_t Application::Exec()
	{
		MSG msg;
		while (::GetMessage(&msg, NULL, 0, 0)){
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return msg.wParam;

		//std::vector<MSG> messageQueue;
		//while (true) {
		//	MSG msg;
		//	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		//		if (msg.message == WM_QUIT) {
		//			return msg.wParam;
		//		}
		//		messageQueue.push_back(msg);
		//	}
		//	for (const MSG& m : messageQueue) {
		//		::TranslateMessage(&m);
		//		::DispatchMessage(&m);
		//	}
		//	//当消息队列为空
		//	//if (messageQueue.empty()) {
		//		//取出全局存储的窗口无效区域
		//	for (auto& it : __LayeredInvalidateRect) {
		//		if (!it.second->IsEmptyArea()) {
		//			//直接进入消息过程进行绘制
		//			::SendMessage(it.first, WM_PAINT, 0, 0);
		//		}
		//	}
		//	//}
		//	messageQueue.clear();
		//	{
		//		//防止CPU空转
		//		if (::GetMessage(&msg, NULL, 0, 0)) {
		//			::TranslateMessage(&msg);
		//			::DispatchMessage(&msg);
		//		}
		//		else {
		//			//退出消息循环
		//			return msg.wParam;
		//		}
		//	}
		//}
		//return 0;
	}

	void Application::Exit(int_t exitCode) {
		::PostQuitMessage(exitCode);
	}

};