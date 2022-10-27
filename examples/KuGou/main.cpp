#include "global.h"
#include "MainFrm.h"
#include "timer.h"
#include "Button.h"

#include "resource.h"


class DesktopWallper :public BorderlessWindow
{
public:
	DesktopWallper(int w, int h, HWND ownerWnd) : BorderlessWindow(w, h, ownerWnd) {};
};
HWND _workerw = nullptr;
//
//inline BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
//{ 
//	HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
//	if (defview != nullptr)
//	{
//		_workerw = FindWindowEx(0, tophandle, L"WorkerW", 0);
//	}
//	return true;
//}//遍历句柄的回调函数
//
//HWND GetWorkerW() {
//	int result;
//	HWND windowHandle = FindWindow(L"Progman", nullptr);
//	SendMessageTimeout(windowHandle, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
//	EnumWindows(EnumWindowsProc, (LPARAM)nullptr);
//	    ShowWindow(_workerw,SW_HIDE);
//	return windowHandle;
//}//获取桌面最底层的句柄
//
void test(int a) {

}

#include <shellscalingapi.h>
#pragma comment(lib,"Shcore.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Curl_Global_Init();

	EzUI::Application app;//相对路径资源
	//SetProcessDPIAware();
	//SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE);

	//ui::Application app(IDR_FILE3,"File", "123456");//载入vs自定义资源

	//Image testGif(L"D:\\test.gif");
	//UINT count = 0;

	//for (size_t i = 0; i < testGif.FrameCount(); i++)
	//{
	//	auto time = testGif.NextFrame();
	//	Sleep(time);
	//}

	MainFrm frm;
	frm.Show();

	return app.exec();

	//HWND workWnd = GetWorkerW();
	//::ShowWindow(workWnd, SW_HIDE);
	/*HWND wnd = GetWorkerW();

	DesktopWallper dw(1920, 1080, wnd);
	dw.SetText(L"EzUI Wallper");
	dw.CloseShadow();
	::SetParent(dw.Hwnd(), wnd);

	Layout l;
	VlcPlayer  player;
	player.Dock = DockStyle::Fill;
	l.AddControl(&player);
	dw.SetLayout(&l);

	::SetWindowPos(wnd, HWND_TOP, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
	dw.Show();*/

	/*player.OpenPath("E:\\CloudMusic\\MV\\x.mp4");
	player.Play();*/

	//Window mainFrm(500, 300);
	//Layout ll;
	//ll.Style.BackgroundColor = Color::Pink;
	//Button btn;
	//btn.SetSize({ 100,35 });
	//btn.SetText("Play");
	//ll.AddControl(&btn);
	//mainFrm.SetLayout(&ll);
	//mainFrm.Show();
	//btn.MouseClick = [&](Control*, MouseButton, const Point&)->void {
	//	player.OpenPath("E:\\CloudMusic\\MV\\x.mp4");
	//	player.Play();
	//};

}