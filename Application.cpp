#include "Application.h"
#include "Window.h"
LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Window* USERDATA = (Window*)UI_GetUserData(hwnd);
	if (USERDATA) {
		return USERDATA->WndProc(message, wParam, lParam);
	}
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

Application::Application() {
	//设计窗口
	::HINSTANCE hInstance = GetModuleHandle(0);
	::WNDCLASS     wc;
	wc.style = NULL;// CS_DBLCLKS;放弃windows消息中的双击消息
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
	if (!RegisterClass(&wc)) //注册窗口
	{
		::MessageBox(NULL, TEXT("This program requires Windows NT!"),
			wc.lpszClassName, MB_ICONERROR);
		return;
	}

	WCHAR exeFullPath[512]{ 0 };
	::GetModuleFileNameW(NULL, exeFullPath, 512);
	std::wstring exeFullPathw(exeFullPath);
	size_t pos = exeFullPathw.rfind(L"\\");
	exeFullPathw = exeFullPathw.substr(0, pos);
	::SetCurrentDirectoryW(exeFullPathw.c_str());

	::CoInitialize(NULL);//初始化com
	::RenderInitialize();
}

Application::~Application() {
	::RenderUnInitialize();
	::CoUninitialize();
}

int Application::exec()
{
	::MSG msg;
	while (GetMessage(&msg, 0, 0, 0) != 0)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


