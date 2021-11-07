#include "Application.h"
#include "Window.h"
//
#include<dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")
LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//if (message == WM_CREATE) {//������Ӱ
	//	const MARGINS shadow = { 0,0,0,0 };
	//	DwmExtendFrameIntoClientArea(hwnd, &shadow);
	//}
	//if (message == WM_NCCALCSIZE) {//ȥ��ϵͳ�˵�
	//	return FALSE;
	//}
	Window *USERDATA = (Window*)UI_GetUserData(hwnd);
	if (USERDATA) {
		return USERDATA->WndProc(message, wParam, lParam);
	}
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

Application::Application() {
	//��ƴ���
	HINSTANCE hInstance = GetModuleHandle(0);
	WNDCLASS     wc;
	wc.style = NULL;
	//wc.style = CS_HREDRAW | CS_VREDRAW;//| CS_DBLCLKS;
	wc.lpfnWndProc = EzUI_WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = UI_CLASSNAME;
	if (!RegisterClass(&wc)) //ע�ᴰ��
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			wc.lpszClassName, MB_ICONERROR);
		return;
	}

	CoInitialize(NULL);//��ʼ��com
	RenderInitialize();
}

Application::~Application() {
	RenderUnInitialize();
	CoUninitialize();
}

int Application::exec()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


