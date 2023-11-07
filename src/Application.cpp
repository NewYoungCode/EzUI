#include "Application.h"
#include <functional>
namespace EzUI {

	LRESULT CALLBACK __EzUI__WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
		if (message == WM_GUI_SYSTEM) {
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
		if (message == WM_GUI_APP) {
			return 0;
		}
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}

	void Application::Init() {
		//����ȫ��ʵ��
		EzUI::__EzUI__HINSTANCE = GetModuleHandleW(NULL);
		//��ƴ���
		::WNDCLASSW    wc{ 0 };
		wc.lpfnWndProc = __EzUI__WndProc;//���ڹ���
		wc.hInstance = EzUI::__EzUI__HINSTANCE;//
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);//���
		wc.lpszClassName = EzUI::__EzUI__WindowClassName;//����
		if (!RegisterClassW(&wc)) //ע�ᴰ��
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		//��ʼ�������ؼ���
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_WIN95_CLASSES;  // ����ʹ��������Ҫ�Ŀؼ����
		::InitCommonControlsEx(&icex);
		//����·��
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
		::CoInitialize(NULL);//��ʼ��com
		RenderInitialize();
	}
	void Application::EnableHighDpi() {
		EzUI::GetMonitor((std::list<MonitorInfo>*) & EzUI::__EzUI__MonitorInfos);
		//DPI��֪���
		//������ϵͳ�Ŵ��޷�����WM_DISPLAYCHANGED��Ϣ
		//bool b = SetProcessDPIAware();
		//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		//�����WM_DPICHANGED��Ϣ�ɽ������п�������
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
		HINSTANCE hInst = EzUI::__EzUI__HINSTANCE;
#ifdef UNICODE
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(resID), custResType.unicode().c_str());
#else
		HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(resID), custResType.c_str());
#endif
		if (!hRsrc)return;
		DWORD len = SizeofResource(hInst, hRsrc);
		EzUI::__EzUI__HVSResource = LoadResource(hInst, hRsrc);
		EzUI::__EzUI__ZipResource = new ZipResource((void*)EzUI::__EzUI__HVSResource, len, password);
	}
	//ʹ�ñ����ļ����Ƽ�����Դ��
	Application::Application(const EString& fileName, const EString& password) {
		Init();
		EzUI::__EzUI__ZipResource = new ZipResource(fileName.unicode().c_str(), password.c_str());
#ifdef _DEBUG
		if (EzUI::__EzUI__ZipResource->IsValid() == false) {
			delete EzUI::__EzUI__ZipResource;
			EzUI::__EzUI__ZipResource = NULL;
			::MessageBoxW(NULL, fileName.unicode().c_str(), L"Failed to open zip", MB_OK | MB_ICONINFORMATION);
		}
#endif
	}
	Application::Application() {
		Init();
	}
	Application::~Application() {
		RenderUnInitialize();
		::CoUninitialize();
		if (EzUI::__EzUI__ZipResource) {
			delete EzUI::__EzUI__ZipResource;
		}
		if (EzUI::__EzUI__HVSResource) {
			FreeResource(EzUI::__EzUI__HVSResource);
		}
		UnregisterClassW(EzUI::__EzUI__WindowClassName, EzUI::__EzUI__HINSTANCE);
	}

	int Application::Exec()
	{
		::MSG msg{ 0 };
		while (::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}

};