#include "Application.h"

namespace EzUI {

	LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		WindowData* wndData = (WindowData*)UI_GetUserData(hwnd);
		if (wndData) {
			return  ((Window*)wndData->Window)->WndProc(message, wParam, lParam);
		}
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	} 

	void Application::Init() {
		//ȷ�����ع����ؼ� DLL ��Comctl32.dll�������� DLL ע���ض��Ĺ����ؼ���
		INITCOMMONCONTROLSEX iccex;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&iccex);

		//��ƴ���
		StdString className = GetThisClassName();
		::HINSTANCE hInstance = GetModuleHandle(NULL);
		::WNDCLASS     wc{ 0 };
		wc.lpfnWndProc = EzUI_WndProc;//���ڹ���
		wc.hInstance = hInstance;//
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);//���
		wc.lpszClassName = className.c_str();//����

		if (!RegisterClass(&wc)) //ע�ᴰ��
		{
			::MessageBox(NULL, TEXT("This program requires Windows NT !"),
				wc.lpszClassName, MB_ICONERROR);
			return;
		}
		::SetCurrentDirectoryW(Application::StartPath().c_str());
		::CoInitialize(NULL);//��ʼ��com
		RenderInitialize();
	}
	Application::Application(int resID, const EString& custResType, const EString& password) {
		Init();
		HINSTANCE hInst = GetModuleHandle(NULL);
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
			::MessageBoxA(NULL, "Open ZipResource Fail !", "Error", 0);
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