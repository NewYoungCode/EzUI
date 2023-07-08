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
		//��ʼ�������ؼ���
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_WIN95_CLASSES;  // ����ʹ��������Ҫ�Ŀؼ����
		InitCommonControlsEx(&icex);
		//����DPI��֪ ����Ӧ�ó����Զ��Ŵ�
		//SetProcessDPIAware();
		//��ƴ���
		::HINSTANCE hInstance = GetModuleHandleW(NULL);
		::WNDCLASSW    wc{ 0 };
		wc.lpfnWndProc = EzUI_WndProc;//���ڹ���
		wc.hInstance = hInstance;//
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);//���
		wc.lpszClassName = WindowClassName;//����

		if (!RegisterClassW(&wc)) //ע�ᴰ��
		{
			::MessageBoxW(NULL, L"This program requires Windows NT !",
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		::SetCurrentDirectoryW(Application::StartPath().c_str());
		::CoInitialize(NULL);//��ʼ��com
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
	//ʹ�ñ����ļ����Ƽ�����Դ��
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