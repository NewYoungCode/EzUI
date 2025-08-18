#include "Application.h"
#include "LayeredWindow.h"
#include <CommCtrl.h>

#undef FindResource
namespace ezui {
	bool g_comInitialized = false;//标记是否由我初始化
	std::list<HWND> g_hWnds;//存储所有使用本框架产生的窗口句柄

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
			g_hWnds.push_back(hWnd);
		}
		else if (message == WM_DESTROY) {
			auto itor = std::find(g_hWnds.begin(), g_hWnds.end(), hWnd);
			if (itor != g_hWnds.end()) {
				g_hWnds.erase(itor);
			}
		}
		WindowData* wndData = (WindowData*)UI_GET_USERDATA(hWnd);
		//执行消息过程
		if (wndData && wndData->WndProc) {
			return wndData->WndProc(hWnd, message, wParam, lParam);
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	void InitInvoker() {
		if (!ezui::__EzUI_MessageWnd) {
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
			wcex.lpszClassName = EZUI_INVOKER_WINDOW_CLASS;
			RegisterClassExW(&wcex);
			ezui::__EzUI_MessageWnd = CreateWindowEx(
				0,                 // 无特殊扩展样式
				EZUI_INVOKER_WINDOW_CLASS,         // 上面注册的类名
				L"",               // 窗口名（无用）
				0,                 // 样式设为 0（无 WS_VISIBLE）
				0, 0, 0, 0,        // 尺寸全为 0
				nullptr, nullptr, ezui::__EzUI__HINSTANCE, nullptr
			);
		}
	}

	//销毁多线程通讯窗口
	void DestroyInvoker() {
		//销毁用于通讯的不可见窗口
		if (ezui::__EzUI_MessageWnd) {
			::DestroyWindow(ezui::__EzUI_MessageWnd);
			ezui::__EzUI_MessageWnd = NULL;
			BOOL ret = UnregisterClassW(EZUI_INVOKER_WINDOW_CLASS, ezui::__EzUI__HINSTANCE);
		}
	}

	void Application::EnableHighDpi() {
		ezui::GetMonitor((std::list<MonitorInfo>*) & ezui::__EzUI__MonitorInfos);//获取一次显示器信息
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
		wcex.lpszClassName = EZUI_WINDOW_CLASS;//类名

		if (!RegisterClassExW(&wcex)) //注册窗口
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wcex.lpszClassName, MB_ICONERROR);
			return;
		}

		//注册一个窗口类并创建隐形窗口用于UI直接的多线程通讯
		InitInvoker();
		//为程序设置工作目录
		std::wstring startPath = Application::StartPath().unicode();
		::SetCurrentDirectoryW(startPath.c_str());
		if (!g_comInitialized) {
			auto ret = ::CoInitialize(NULL);//初始化com
			if (ret == S_OK) {
				g_comInitialized = true;//由我初始化的
			}
		}
		RenderInitialize();//初始化图形绘制库 D2D/GDI/GDI+
	}
	//销毁所有窗口
	void DestroyAllWindows() {
		while (ezui::g_hWnds.size() > 0)
		{
			auto itor = ezui::g_hWnds.begin();
			if (itor != ezui::g_hWnds.end()) {
				HWND hwnd = *itor;
				g_hWnds.erase(itor);
				::DestroyWindow(hwnd);
			}
		}
	}

	Application::~Application() {
		//销毁通讯窗口
		DestroyInvoker();
		//销毁所有窗口
		DestroyAllWindows();
		//取消窗口注册的类
		BOOL ret = UnregisterClassW(EZUI_WINDOW_CLASS, ezui::__EzUI__HINSTANCE);
		RenderUnInitialize();
		if (g_comInitialized) {
			::CoUninitialize();
			g_comInitialized = false;
		}
		if (ezui::__EzUI__Resource) {
			delete ezui::__EzUI__Resource;
			ezui::__EzUI__Resource = NULL;
		}
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
		//销毁用于通讯的不可见窗口
		DestroyInvoker();
		//销毁所有窗口
		DestroyAllWindows();
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