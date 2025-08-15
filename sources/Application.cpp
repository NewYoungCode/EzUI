#include "Application.h"
#include "LayeredWindow.h"
#include <CommCtrl.h>

#undef FindResource
namespace ezui {
	// 内部使用：枚举名称时的上下文
	struct ResourceContext {
		UIString rcIDName;
		HRSRC hResource = NULL;
		UIString rcType;
	};
	// 回调：枚举资源名称
	BOOL CALLBACK EnumNamesProc(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam) {
		ResourceContext* ctx = (ResourceContext*)(lParam);
		UIString resName = UIString(lpszName).toLower();
		if (!IS_INTRESOURCE(lpszName) && ctx->rcIDName.toLower() == resName) {
			ctx->hResource = FindResourceW(hModule, lpszName, lpszType);
			ctx->rcType = (wchar_t*)lpszType;
			return FALSE; // 找到就停止
		}
		return TRUE; // 继续
	}
	//通过资源名称查找windows嵌套资源
	HRSRC FindResource(const UIString& rcIDName)
	{
		HMODULE hModule = ezui::__EzUI__HINSTANCE;
		ResourceContext ctx;
		ctx.rcIDName = rcIDName;
		EnumResourceTypesW(hModule, [](HMODULE hModule, LPWSTR lpszType, LONG_PTR lParam)->BOOL {
			return EnumResourceNamesW(hModule, lpszType, EnumNamesProc, lParam);
			}, (LONG_PTR)&ctx);
		return ctx.hResource;
	}
}

namespace ezui {
	LRESULT CALLBACK __EzUI__WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		if (message == WM_CREATE) {
			__EzUI__Wnds.push_back(hWnd);
		}
		else if (message == WM_DESTROY) {
			auto itor = std::find(__EzUI__Wnds.begin(), __EzUI__Wnds.end(), hWnd);
			if (itor != __EzUI__Wnds.end()) {
				__EzUI__Wnds.erase(itor);
			}
		}
		WindowData* wndData = (WindowData*)UI_GET_USERDATA(hWnd);
		//执行消息过程
		if (wndData && wndData->WndProc) {
			return wndData->WndProc(hWnd, message, wParam, lParam);
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	const wchar_t* __EzUI__HiddenMessageWindowClass = L"__EzUI__HiddenMessageWindowClass";
	void RegMessageWnd() {
		::WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(wcex);
		wcex.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
			if (msg == WM_GUI_SYSTEM) {
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
			return ::DefWindowProc(hwnd, msg, wParam, lParam);
			};
		wcex.hInstance = ezui::__EzUI__HINSTANCE;
		wcex.lpszClassName = __EzUI__HiddenMessageWindowClass;
		RegisterClassExW(&wcex);
		ezui::__EzUI_MessageWnd = CreateWindowEx(
			0,                 // 无特殊扩展样式
			__EzUI__HiddenMessageWindowClass,         // 上面注册的类名
			L"",               // 窗口名（无用）
			0,                 // 样式设为 0（无 WS_VISIBLE）
			0, 0, 0, 0,        // 尺寸全为 0
			nullptr, nullptr, ezui::__EzUI__HINSTANCE, nullptr
		);
	}

	void Application::EnableHighDpi() {
		ezui::GetMonitor((std::list<MonitorInfo>*) & ezui::__EzUI__MonitorInfos);
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

	bool Application::SetResource(const UIString& localOrResName) {
		//只允许有一个资源文件
		if (ezui::__EzUI__Resource) {
			delete ezui::__EzUI__Resource;
			ezui::__EzUI__Resource = NULL;
		}
		//先从vs中的资源里面查找
		bool found = false;
		HRSRC hRsrc = ezui::FindResource(localOrResName);
		if (hRsrc) {
			ezui::__EzUI__Resource = new Resource(hRsrc);
			found = true;
		}
		else {
			//本地文件中获取
			std::wstring wstr = localOrResName.unicode();
			DWORD dwAttr = GetFileAttributesW(wstr.c_str());
			if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
				ezui::__EzUI__Resource = new Resource(localOrResName);
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
	Application::Application(HINSTANCE hInstance) {
		//存入全局实例
		ezui::__EzUI__HINSTANCE = hInstance == NULL ? ::GetModuleHandleW(NULL) : hInstance;
		ezui::__EzUI__ThreadId = ::GetCurrentThreadId();
		//设计窗口
		::WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;//宽高改变窗口触发重绘
		wcex.lpfnWndProc = __EzUI__WndProc;//窗口过程
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = ezui::__EzUI__HINSTANCE;//
		wcex.hIcon = NULL;//(大图标)用于窗口标题栏、Alt+Tab 切换界面、任务栏预览等显示大图标的地方
		wcex.hIconSm = NULL;//(小图标)用于任务栏按钮、系统托盘（部分情况下）以及 Alt+Tab 小图标显示等
		wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);//光标
		wcex.hbrBackground = NULL; // 窗口背景
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = ezui::__EzUI__WindowClassName;//类名

		if (!RegisterClassExW(&wcex)) //注册窗口
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wcex.lpszClassName, MB_ICONERROR);
			return;
		}

		//注册一个窗口类并创建隐形窗口用于UI通讯
		RegMessageWnd();

		//初始化公共控件库
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_WIN95_CLASSES;  // 或者使用其他需要的控件类别
		::InitCommonControlsEx(&icex);
		//为程序设置工作目录
		std::wstring startPath = Application::StartPath().unicode();
		::SetCurrentDirectoryW(startPath.c_str());
		::CoInitialize(NULL);//初始化com
		RenderInitialize();//初始化图形绘制库 D2D/GDI/GDI+
	}
	Application::~Application() {
		RenderUnInitialize();
		::CoUninitialize();
		if (ezui::__EzUI__Resource) {
			delete ezui::__EzUI__Resource;
			ezui::__EzUI__Resource = NULL;
		}
		UnregisterClassW(ezui::__EzUI__HiddenMessageWindowClass, ezui::__EzUI__HINSTANCE);
		UnregisterClassW(ezui::__EzUI__WindowClassName, ezui::__EzUI__HINSTANCE);
	}

	int Application::Exec()
	{
		MSG msg;
		while (::GetMessage(&msg, NULL, 0, 0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return msg.wParam;
	}

	void Application::Exit(int exitCode) {
		//销毁通讯窗口
		::DestroyWindow(ezui::__EzUI_MessageWnd);
		//退出循环前 销毁所有窗口
		while (ezui::__EzUI__Wnds.size() > 0)
		{
			auto itor = ezui::__EzUI__Wnds.begin();
			if (itor != ezui::__EzUI__Wnds.end()) {
				HWND hwnd = *itor;
				__EzUI__Wnds.erase(itor);
				::DestroyWindow(hwnd);
			}
		}
		//退出消息循环
		::PostQuitMessage(exitCode);
	}

	UIString Application::StartPath()
	{
		std::vector<wchar_t> wPath(32768);
		DWORD count = ::GetModuleFileNameW(__EzUI__HINSTANCE, wPath.data(), (DWORD)wPath.size());
		for (int i = count - 1; i > -1; i--)
		{
			if (wPath[i] == L'\\') {
				wPath[i] = 0;
				break;
			}
		}
		return UIString(wPath.data());
	}

	bool Application::GetResource(const UIString& filename, std::string* outFileData)
	{
		outFileData->clear();
		//本地文件中获取
		std::wstring wstr = filename.unicode();
		DWORD dwAttr = GetFileAttributesW(wstr.c_str());
		if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
			std::ifstream ifs(wstr, std::ios::binary);
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			outFileData->resize(size);
			ifs.seekg(0);
			ifs.read((char*)outFileData->c_str(), size);
			ifs.close();
			return true;
		}
		//从资源中获取
		if (ezui::__EzUI__Resource) {
			if (ezui::__EzUI__Resource->GetFile(filename, outFileData)) {
				return true;
			}
		}
		return false;
	}

};